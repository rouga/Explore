#include <iostream>

#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Window.h"


#define FMT_UNICODE 0
#include "spdlog/spdlog.h"

int main()
{
	Editor wEditor;

	wEditor.Initialize();
	wEditor.Run();
	wEditor.Shutdown();

	return EXIT_SUCCESS;
}