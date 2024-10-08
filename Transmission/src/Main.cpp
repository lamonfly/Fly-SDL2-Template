#include "Core/Engine.h"
#include "SampleScene.h"
#include "BackgroundScene.h"

#include <stdio.h>

int main(int argc, char* args[])
{
	if (!Engine::GetInstance()->Init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		Engine::GetInstance()->AddScene<BackgroundScene>("background");
		Engine::GetInstance()->LoadScene("background", "background");
		Engine::GetInstance()->AddScene<SampleScene>("game");
		Engine::GetInstance()->LoadScene("game", "game");

		Engine::GetInstance()->GetWindow()->Color = { .r = 0, .g = 0, .b = 0 };

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