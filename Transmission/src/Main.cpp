#include "Core/Engine.h"
#include "SampleScene.h"

#include <stdio.h>

int main(int argc, char* args[])
{
	if (!Engine::GetInstance()->Init(new SampleScene()))
	{
		printf("Failed to initialize!\n");
	}
	else
	{
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