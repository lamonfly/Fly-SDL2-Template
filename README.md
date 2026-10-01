# Fly-SDL2-Template

A C++ game engine template built on SDL2, designed to give you a solid starting point for 2D games with an Entity-Component System (ECS) architecture, [Jolt Physics](https://github.com/jrouwe/JoltPhysics) for rigid bodies and collisions, and a modular scene framework.

The repository ships with a fully working **Breakout** clone called *Transmission* that demonstrates all major engine features.

---

## Features

- **Entity-Component System** via [EnTT](https://github.com/skypjack/entt) — per-scene ECS registries with type-safe component access
- **Scene system** — multiple scenes can be active simultaneously; scenes can be loaded, replaced, and removed at runtime through a deferred task queue (safe to call from within `Update` or `HandleEvent`)
- **Physics & collisions** via [Jolt Physics](https://github.com/jrouwe/JoltPhysics) (MIT) — one `PhysicsWorld` per scene
  - `Transform` (position, rotation, scale, size) and `RigidBody` (box or circle; static, kinematic or dynamic) components
  - 2D simulation on a 3D engine: bodies are locked to the XY plane (`EAllowedDOFs::Plane2D`), so a later move to 3D keeps the same library
  - **Game-defined collision layers**: 32-bit `Layer` / `CollidesWith` masks per body, declared by each project (the engine ships optional defaults); mapped onto Jolt object layers automatically
  - **Contact events** per entity (`Enter`, `Stay`, `Exit`) with contact point and normal
  - **Continuous Collision Detection** per body (`UseCCD = true`, Jolt `LinearCast`) for fast-moving objects
  - Fixed 60 Hz time step with accumulator, multi-threaded solver, sensors, restitution/friction/damping/gravity factor per body
- **Graphics** — `Sprite`, `Circle`, `Line`, `Text`, and `Texture` components rendered via `SDL_Renderer`
- **Audio** — SDL_mixer initialised at engine startup (44100 Hz, stereo)
- **Event system** — `Eventable` interface; built-in `Grab` component for mouse-drag interactions
- **Window management** — logical resolution (720 × 480 by default) independent of physical display size; resizable, maximised on launch; fullscreen toggle via **Enter**
- **Delta-time game loop** using `SDL_GetPerformanceCounter`

---

## Prerequisites

| Requirement | Notes |
|---|---|
| Visual Studio 2022 (v143) | Windows only. Install the **Desktop development with C++** workload, which includes vcpkg |
| x64 architecture | |
| NuGet | SDL2 and EnTT restore automatically on first build |
| vcpkg (bundled with VS 2022) | Jolt Physics is pulled through `vcpkg.json` (manifest mode). One-time setup per machine, from a **Developer PowerShell for VS 2022**: `vcpkg integrate install` |

**NuGet packages used:** `sdl2.nuget`, `sdl2_image.nuget`, `sdl2_ttf.nuget`, `sdl2_mixer.nuget`, `fluid.entt`

**vcpkg packages used:** `joltphysics` (first build compiles or downloads Jolt into `vcpkg_installed/`, which is git-ignored; `Jolt.dll` is copied next to the executable automatically)

---

## Getting Started

```bash
git clone https://github.com/your-username/Fly-SDL2-Template.git
```

1. Once per machine, open a *Developer PowerShell for VS 2022* and run `vcpkg integrate install`.
2. Open `GameTemplate.sln` in Visual Studio 2022.
3. Right-click **Transmission** in Solution Explorer → *Set as Startup Project*.
4. Select the **x64 | Debug** configuration.
5. Press **F5** — NuGet restores SDL2/EnTT, vcpkg installs Jolt, and the Breakout demo launches.

---

## Project Structure

```
GameTemplate.sln
├── vcpkg.json                # vcpkg manifest (Jolt Physics)
├── Jolt.props                # Shared MSBuild sheet: vcpkg manifest mode + C++ standard
├── Engine/                   # Static library (.lib) — the reusable game engine
│   └── src/
│       ├── Core/             # Engine singleton, Window
│       ├── Scene/            # Scene base class (owns a PhysicsWorld), Camera
│       ├── Physics/          # Transform, RigidBody, Contact, PhysicsWorld (Jolt wrapper),
│       │                     # PhysicsConfig, CollisionLayer, Layers (Jolt layer filters), JoltGlobals
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
#include <Physics/RigidBody.h>
#include <Physics/CollisionLayer.h>
#include <Physics/PhysicsConfig.h>
#include <Graphics/Sprite.h>

class MyScene : public Scene
{
public:
    void Init() override
    {
        // Configure physics (optional — must happen before the first RigidBody)
        PhysicsConfig cfg;
        cfg.PixelsPerMeter = 32.0f;            // Jolt works in meters
        cfg.Gravity        = Vector2(0, 9.81f); // Y down, like SDL
        SetPhysicsConfig(cfg);

        // Create a static wall entity (Transform first, then RigidBody)
        auto wall = mRegistry.create();
        mRegistry.emplace<Transform>(wall, Vector2(0.0f, 680.0f));
        auto body = RigidBody::Box(Vector2(1280, 32), BodyType::Static);
        body.Layer        = CollisionLayer::Default;   // or your own game layers, see below
        body.CollidesWith = CollisionLayer::All;
        mRegistry.emplace<RigidBody>(wall, body); // Jolt body is created automatically

        // A bouncing dynamic ball
        auto ball = mRegistry.create();
        mRegistry.emplace<Transform>(ball, Vector2(600.0f, 100.0f));
        auto ballBody = RigidBody::Circle(8.0f, BodyType::Dynamic);
        ballBody.Restitution = 0.8f;
        ballBody.UseCCD      = true;
        mRegistry.emplace<RigidBody>(ball, ballBody);
        GetPhysics().SetLinearVelocity(mRegistry.get<RigidBody>(ball), Vector2(120.0f, 0.0f)); // px/s
    }

    void Update(double deltaTime) override
    {
        // Engine::Update already called UpdatePhysics; read the results here
        for (auto&& [entity, body] : mRegistry.view<RigidBody>().each()) {
            for (const Contact& c : GetContacts(entity)) {
                if (c.Phase == ContactPhase::Enter) { /* c.Other, c.Point (px), c.Normal */ }
            }
        }
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

### Physics (Jolt)
Each `Scene` owns a `PhysicsWorld` wrapping a `JPH::PhysicsSystem`. `Engine::Update` calls `Scene::UpdatePhysics` before `Scene::Update` every frame.

| Component | Description |
|---|---|
| `Transform` | Position (`Vector2`, top-left corner in pixels), rotation (0–360°, clockwise), scale, and size |
| `RigidBody` | Box or circle body. `BodyType::Static` never moves, `Kinematic` follows the `Transform` you write, `Dynamic` is simulated and writes the `Transform` back |

Body lifecycle is automatic: `mRegistry.emplace<RigidBody>(entity, ...)` creates the Jolt body (the entity must already have a `Transform`), and `remove<RigidBody>` / `destroy(entity)` destroys it.

Per-body settings: `Restitution`, `Friction`, `LinearDamping`, `AngularDamping`, `GravityFactor`, `UseCCD`, `IsSensor`, `AllowSleeping`, `LockRotation`.

#### Collision layers
Filtering uses two 32-bit masks per body. Two bodies collide when `(A.Layer & B.CollidesWith) && (B.Layer & A.CollidesWith)`. The engine attaches no meaning to the bits, so **each game declares its own layers** without touching `Engine/`:

```cpp
// MyGame/src/MyLayers.h
namespace MyLayer { enum : uint32_t { Ship = 1 << 0, Asteroid = 1 << 1, Bullet = 1 << 2 }; }

body.Layer        = MyLayer::Bullet;
body.CollidesWith = MyLayer::Asteroid | MyLayer::Ship;   // unscoped enum: no casts needed
```

`Physics/CollisionLayer.h` ships Unity's built-in layers at the same indices (`CollisionLayer::Default` 0, `TransparentFX` 1, `IgnoreRaycast` 2, `Water` 4, `UI` 5, plus `All` and `None`); bits 6 to 31 are free for games, Godot-style, through `LayerBit(n)`. Up to 32 layers. Internally every distinct `(Layer, CollidesWith, static?)` combination is interned into a Jolt `ObjectLayer` (`Physics/Layers.h`); static bodies go to the static broad-phase layer, everything else to the moving one.

Velocity is read and written in pixels per second through the world: `GetPhysics().SetLinearVelocity(body, v)`, `GetLinearVelocity(body)`, `SetAngularVelocity(body, degPerSec)`. `GetPhysics().GetBodyInterface()` exposes raw Jolt when needed.

### Contacts
`GetContacts(entity)` returns the `Contact` list produced during the last `UpdatePhysics`:

| Field | Meaning |
|---|---|
| `Other` | The other entity |
| `Point` | Contact point in pixels (average of the manifold) |
| `Normal` | Unit normal from this entity towards `Other` |
| `Phase` | `Enter` (new this frame), `Stay`, `Exit` (ended; `Point`/`Normal` are zero) |

Contacts are collected on Jolt worker threads and handed out after the step, so game code never runs inside Jolt callbacks.

### Units, time step and CCD
`PhysicsConfig` (set with `SetPhysicsConfig` before the first body) holds `PixelsPerMeter` (default 32), `Gravity` (m/s², Y down), `FixedTimeStep` (1/60 s), `MaxSubSteps` (4) and the Jolt capacity limits. The world steps in fixed increments from an accumulator, so rendering frame rate does not affect simulation.

Set `UseCCD = true` on fast dynamic bodies; Jolt then uses `LinearCast` motion quality to prevent tunnelling.

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

- **`SampleScene`** — game-specific layers in `Breakout/BreakoutLayers.h` (`Ball`, `Paddle`, `Brick`, `Wall`); a grid of static brick bodies with `Sprite` and a `Brick` behaviour component; a kinematic paddle driven by `PlayerPlatform` (keyboard-controlled); a dynamic ball (`Restitution = 1`, `Friction = 0`, CCD on) that Jolt bounces, with `BallMovement` speeding it up on every `Enter` contact
- **`GameOverScene`** — a minimal scene with centred `Text`; pressing any key reloads `SampleScene`
- Zero-gravity `PhysicsConfig`, contact events used for brick damage and the game-over trigger (ball touches the top wall)
- Per-entity UV clipping to sample random sub-regions of a tileable texture atlas

Run it out of the box by pressing **F5** in Visual Studio with **Transmission** set as the startup project.

---

## License

MIT License — free to use, modify, and distribute with attribution.

Third-party: [Jolt Physics](https://github.com/jrouwe/JoltPhysics) (MIT, Jorrit Rouwe), [EnTT](https://github.com/skypjack/entt) (MIT), [SDL2](https://www.libsdl.org/) (zlib).

```
MIT License

Copyright (c) 2026 lamonfly

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
