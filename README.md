# Fly-SDL2-Template

A C++ game engine template built on SDL2, designed to give you a solid starting point for 2D games with an Entity-Component System (ECS) architecture, a physics/collision system, and a modular scene framework.

The repository ships with a fully working **Breakout** clone called *Transmission* that demonstrates all major engine features.

---

## Features

- **Entity-Component System** via [EnTT](https://github.com/skypjack/entt) — per-scene ECS registries with type-safe component access
- **Scene system** — multiple scenes can be active simultaneously; scenes can be loaded, replaced, and removed at runtime through a deferred task queue (safe to call from within `Update` or `HandleEvent`)
- **Physics & collisions**
  - `Transform` (position, rotation, scale, size) and `Velocity` components
  - `Collider` with `RectShape` and `CircleShape`
  - Bitmasked **collision layers** (`Default`, `Player`, `Enemy`, `Projectile`, `World`, `Trigger`, `PowerUp`, `Debris`)
  - **Continuous Collision Detection (CCD)** — sweep tests (`Circle–Circle`, `Circle–AABB`, `AABB–AABB`) for fast-moving objects
  - **Broad phase** with pluggable **spatial partitioning** (`SpatialGrid` or `QuadTree`), optional static/dynamic separation for better performance
- **Graphics** — `Sprite`, `Circle`, `Line`, `Text`, and `Texture` components rendered via `SDL_Renderer`
- **Audio** — SDL_mixer initialised at engine startup (44100 Hz, stereo)
- **Event system** — `Eventable` interface; built-in `Grab` component for mouse-drag interactions
- **Window management** — logical resolution (720 × 480 by default) independent of physical display size; resizable, maximised on launch; fullscreen toggle via **Enter**
- **Delta-time game loop** using `SDL_GetPerformanceCounter`

---

## Prerequisites

| Requirement | Notes |
|---|---|
| Visual Studio 2022 (v143) | Windows only |
| x64 architecture | |
| NuGet | Packages restore automatically on first build — no manual steps needed |

**NuGet packages used:** `sdl2.nuget`, `sdl2_image.nuget`, `sdl2_ttf.nuget`, `sdl2_mixer.nuget`, `fluid.entt`

---

## Getting Started

```bash
git clone https://github.com/your-username/Fly-SDL2-Template.git
```

1. Open `GameTemplate.sln` in Visual Studio 2022.
2. Right-click **Transmission** in Solution Explorer → *Set as Startup Project*.
3. Select the **x64 | Debug** configuration.
4. Press **F5** — NuGet restores all packages and the Breakout demo launches.

---

## Project Structure

```
GameTemplate.sln
├── Engine/                   # Static library (.lib) — the reusable game engine
│   └── src/
│       ├── Core/             # Engine singleton, Window
│       ├── Scene/            # Scene base class, Camera
│       ├── Physics/          # Transform, Velocity, Collider, CCD, spatial partitioning
│       │   └── Collider/     # Collider, shapes (Rect/Circle), collision layers, broad phase
│       ├── Graphics/         # Sprite, Circle, Line, Text, Texture
│       └── Event/            # Eventable interface, Grab component
├── Transmission/             # Executable — the Breakout demo game
│   └── src/
│       ├── Main.cpp          # Engine bootstrap
│       ├── SampleScene.h     # Gameplay scene (bricks, ball, paddle)
│       ├── GameOverScene.h   # Game-over screen
│       └── Breakout/         # Game-specific components (BallMovement, Brick, PlayerPlatform)
└── packages/                 # Restored NuGet packages
```

---

## Using the Engine

### 1. Create a scene

Subclass `Scene` and implement the four pure-virtual methods. The base class exposes the EnTT registry (`mRegistry`) and three iteration helpers:

```cpp
#include "Scene/Scene.h"
#include <Physics/Transform.h>
#include <Physics/Velocity.h>
#include <Physics/Collider/Collider.h>
#include <Physics/Collider/Shapes/RectShape.h>
#include <Physics/Collider/CollisionLayer.h>
#include <Physics/SpatialPartitioning/SpatialPartitionConfig.h>
#include <Graphics/Sprite.h>

class MyScene : public Scene
{
public:
    void Init() override
    {
        // Configure spatial partitioning (optional — defaults to a 64px grid)
        SpatialPartitionConfig cfg;
        cfg.Strategy              = SpatialPartitionStrategy::Grid;
        cfg.WorldSize             = Vector2(1280, 720);
        cfg.GridCellSize          = 64.0f;
        cfg.SeparateStaticDynamic = true;
        SetSpatialPartitionConfig(cfg);

        // Create a static wall entity
        auto wall = mRegistry.create();
        mRegistry.emplace<Transform>(wall, Vector2(0.0f, 680.0f));
        auto& col = mRegistry.emplace<Collider>(wall, new RectShape(Vector2(1280, 32)));
        col.IsStatic     = true;
        col.Layer        = CollisionLayer::World;
        col.CollidesWith = static_cast<uint16_t>(CollisionLayer::Player | CollisionLayer::Projectile);
    }

    void Update(double deltaTime) override
    {
        UpdatePhysics(deltaTime);          // runs broad phase + collision resolution
        UpdateType<MyBehavior>(deltaTime); // calls MyBehavior::Update on every entity that has one
    }

    void Render(SDL_Renderer* renderer) override
    {
        RenderType<Sprite>(renderer);
        RenderType<Circle>(renderer);
        RenderType<Text>(renderer);
    }

    void HandleEvent(SDL_Event& e) override
    {
        HandleEventType<MyBehavior>(e);
    }
};
```

### 2. Bootstrap the engine

```cpp
#include "Core/Engine.h"
#include "MyScene.h"

int main(int argc, char* args[])
{
    if (!Engine::GetInstance()->Init())
        return 1;

    Engine::GetInstance()->AddScene<MyScene>("MyScene");
    Engine::GetInstance()->LoadScene("MyScene", "MyScene");

    // Optional: set window background colour
    Engine::GetInstance()->GetWindow()->Color = { 30, 30, 30, 255 };

    while (Engine::GetInstance()->IsRunning())
    {
        Engine::GetInstance()->Events();
        Engine::GetInstance()->Update();
        Engine::GetInstance()->Render();
    }

    Engine::GetInstance()->Clean();
    return 0;
}
```

### 3. Switch scenes at runtime

```cpp
// Safe to call from HandleEvent or Update — executed via a deferred task queue
Engine::GetInstance()->LoadScene("GameOver", "GameOver");
Engine::GetInstance()->RemoveScene("MyScene");
```

---

## Core Systems

### ECS (Entity-Component System)
Each `Scene` owns an `entt::registry`. Entities are opaque integers; behaviour is entirely data-driven through attached components. The `Scene` base class provides three templated helpers that iterate all entities carrying a given component type and forward the call to it:

| Helper | Calls on component |
|---|---|
| `RenderType<T>(renderer)` | `T::Render(SDL_Renderer*, Transform)` |
| `UpdateType<T>(deltaTime)` | `T::Update(double)` |
| `HandleEventType<T>(event)` | `T::HandleEvent(SDL_Event&)` |

### Scene Management
`Engine::AddScene<T>("id")` registers a factory for scene `T`. `LoadScene("id", "activeId")` constructs and initialises a new instance; `RemoveScene("id")` destroys it. All scene operations are deferred to the end of the current frame, so it is safe to trigger them from any context.

### Physics
| Component | Description |
|---|---|
| `Transform` | Position (`Vector2`), rotation (0–360°), scale, and size |
| `Velocity` | `Vector2` applied to `Transform::Position` each frame |
| `Collider` | Wraps a `Shape*`; set `IsStatic = true` for immovable geometry |

Collision filtering is done with two bitmask fields on `Collider`:

```cpp
col.Layer        = CollisionLayer::Player;
col.CollidesWith = static_cast<uint16_t>(CollisionLayer::World | CollisionLayer::Enemy);
```

### Continuous Collision Detection (CCD)
Enable per-collider with `UseCCD = true` and tune `CCDSpeedThreshold` (pixels/second). When an entity exceeds the threshold, `UpdatePhysics` uses one of three sweep tests instead of a discrete overlap check:

| Method | Typical use |
|---|---|
| `CCD::SweepCircleCircle` | Ball vs ball |
| `CCD::SweepCircleAABB` | Ball vs rectangular obstacle |
| `CCD::SweepAABBAABB` | Box vs box |

### Spatial Partitioning
Configured via `SpatialPartitionConfig` before the scene finishes `Init`:

| Strategy | Class | Best for |
|---|---|---|
| `SpatialPartitionStrategy::Grid` | `SpatialGrid` | Uniformly distributed objects |
| `SpatialPartitionStrategy::QuadTree` | `QuadTree` | Clustered or sparse worlds |

`SeparateStaticDynamic = true` builds a second partition for static colliders so they are never re-inserted on moving frames.

### Graphics
All renderable types implement `Renderable` and expose `Render(SDL_Renderer*, Transform)`:

| Component | Description |
|---|---|
| `Sprite` | Renders a `Texture` with optional UV clip rect and horizontal/vertical flip |
| `Circle` | Draws a filled or outlined circle |
| `Line` | Draws a line segment |
| `Text` | Renders TTF text via SDL_ttf |

`Texture` wraps `SDL_Texture` and supports:
- `LoadFromFile(path)` — load PNG/JPG/etc.
- `LoadFromFile(path, colorKey)` — load with colour-key transparency
- `LoadText(font, text, color)` — render a TTF string to a texture
- `SetAlpha`, `SetColor`, `SetBlendMode`, `SetSize`

### Audio
SDL_mixer is initialised by `Engine::Init` at 44100 Hz, stereo, with a 2048-sample buffer. Use the SDL_mixer API directly to load and play sounds:

```cpp
Mix_Chunk* sfx = Mix_LoadWAV("res/bounce.wav");
Mix_PlayChannel(-1, sfx, 0);
```

### Window
`Window` creates a maximised, resizable SDL window. The logical resolution defaults to **720 × 480** and scales independently of the physical window size.

| Method | Description |
|---|---|
| `GetWidth()` / `GetHeight()` | Physical window dimensions |
| `GetResolutionWidth()` / `GetResolutionHeight()` | Logical (game) dimensions |
| `GetMouseLogicalPosition()` | Mouse position mapped to logical coordinates |
| `Color` | Background clear colour (`SDL_Color`, default white) |

Press **Enter** at any time to toggle fullscreen.

---

## Sample Game: Transmission

*Transmission* is a Breakout clone included as the primary consumer of the engine. It demonstrates:

- **`SampleScene`** — a grid of brick entities with `Sprite`, `Collider` (static, `World` layer), and a `Brick` behaviour component; a paddle driven by `PlayerPlatform` (keyboard-controlled); a ball with `BallMovement` and CCD enabled (`CCDSpeedThreshold = 200`)
- **`GameOverScene`** — a minimal scene with centred `Text`; pressing any key reloads `SampleScene`
- A `SpatialGrid` with static/dynamic separation for efficient per-frame collision queries across many brick entities
- Per-entity UV clipping to sample random sub-regions of a tileable texture atlas

Run it out of the box by pressing **F5** in Visual Studio with **Transmission** set as the startup project.

---

## License

Add your license here.
