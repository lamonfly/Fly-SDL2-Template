#include "Core/Engine.h"
#include "SampleScene.h"
#include "GameOverScene.h"

#include <stdio.h>

int main(int argc, char* args[])
{
	if (!Engine::GetInstance()->Init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		srand(time(NULL));

		Engine::GetInstance()->AddScene<SampleScene>("SampleScene");
		Engine::GetInstance()->AddScene<GameOverScene>("GameOverScene");
		Engine::GetInstance()->LoadScene("SampleScene", "SampleScene");

		Engine::GetInstance()->GetWindow()->Color = { .r = 200, .g = 233, .b = 233 };

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