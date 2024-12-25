#pragma once

#include <vulkan/vulkan.h>

class Instance
{
public:
	Instance();
	~Instance();

	void Initialize(const char* iAppName);

	VkInstance GetInstance() const { return mInstance; }

private:
	VkInstance mInstance = nullptr;
};