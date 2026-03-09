#include "main.h"

#include <iostream>
#include <windows.h>
#include "Engine/Engine.h"
#include "App.h"

void AttachConsoleIO()
{
	AllocConsole();

	FILE* f;
	freopen_s(&f, "CONIN$", "r", stdin);
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONOUT$", "w", stderr);
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{

	//AttachConsoleIO();

	Engine engine;
	App app(engine);

	engine.Initialize();
	engine.Run();
	engine.Shutdown();


    return 0;
}
