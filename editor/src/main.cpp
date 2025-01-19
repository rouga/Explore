#include <iostream>

#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Window.h"
#include "Core/Logger.h"

int main()
{
	Logger& sLogger = Logger::Get();
	sLogger.Initialize(true);
	Editor wEditor;

	wEditor.Initialize();
	wEditor.Run();
	wEditor.Shutdown();

	return EXIT_SUCCESS;
}