#pragma once

#include <memory>

#include "vulkan/Instance.h"

class Engine 
{
public:
    void Initialize();
		void Run();

private:
	std::unique_ptr<Instance> mInstance;
};
