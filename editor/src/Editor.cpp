#include <iostream>
#include "Engine.h"
#include "Window.h"

int main() 
{
    std::cout << "Launching Editor..." << std::endl;
    Engine wEngine;
		Window wWnd;

    try 
		{
			wWnd.Initialize(1920, 1080, "Editor");
			wEngine.Initialize();
    } 
		catch (const std::exception& e) 
		{
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

		while (!wWnd.ShouldClose())
		{
			wEngine.Run();
		}

    std::cout << "Editor closed successfully." << std::endl;
    return EXIT_SUCCESS;
}