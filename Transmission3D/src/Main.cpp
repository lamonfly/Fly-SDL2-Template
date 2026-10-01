#include "Core/Engine.h"
#include "SampleScene3D.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* args[])
{
	EngineConfig config;
	config.Backend = RenderBackend::OpenGL3;
	config.MSAASamples = 4;

	if (!Engine::GetInstance()->Init(config))
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		srand(static_cast<unsigned>(time(NULL)));

		Engine::GetInstance()->AddScene<SampleScene3D>("SampleScene3D");
		Engine::GetInstance()->LoadScene("SampleScene3D", "SampleScene3D");

		Engine::GetInstance()->GetWindow()->Color = { .r = 28, .g = 30, .b = 38, .a = 255 };

		while (Engine::GetInstance()->IsRunning())
		{
			Engine::GetInstance()->Events();
			Engine::GetInstance()->Update();
			Engine::GetInstance()->Render();
		}
	}

	Engine::GetInstance()->Clean();
	return 0;
}
