#pragma once

#include <memory>

#include "graphics/VulkanInstance.h"

class Engine 
{
public:
    void Initialize();
		void Run();

private:
	std::unique_ptr<VulkanInstance> mInstance;
};
