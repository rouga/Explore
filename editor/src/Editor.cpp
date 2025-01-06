#include <iostream>
#include "Engine.h"
#include "Window.h"

#define FMT_UNICODE 0
#include "spdlog/spdlog.h"

int main() 
{
		spdlog::info("Launching Editor...");
		Window wWnd(&Engine::Get());

    try 
		{
			wWnd.Initialize(1920, 1080, "Editor");
			Engine::Get().Initialize(&wWnd);
    } 
		catch (const std::exception& e) 
		{
			spdlog::error("Error: {:s}", e.what());
        return EXIT_FAILURE;
    }

		while (!wWnd.ShouldClose())
		{
			Engine::Get().Run();
		}

		spdlog::info("Closing Editor.");
		Engine::Get().Shutdown();

    return EXIT_SUCCESS;
}