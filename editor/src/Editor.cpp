#include <iostream>
#include "Engine.h"
#include "Window.h"

#define FMT_UNICODE 0
#include "spdlog/spdlog.h"

int main() 
{
		spdlog::info("Launching Editor...");
		Window wWnd;
		Engine wEngine{&wWnd};

    try 
		{
			wWnd.Initialize(1920, 1080, "Editor");
			wEngine.Initialize();
    } 
		catch (const std::exception& e) 
		{
			spdlog::error("Error: {:s}", e.what());
        return EXIT_FAILURE;
    }

		while (!wWnd.ShouldClose())
		{
			wEngine.Run();
		}

		spdlog::info("Closing Editor.");

    return EXIT_SUCCESS;
}