#include "Core/Engine.h"

int main(int argc, char* args[])
{
	if (!Engine::GetInstance()->Init())
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