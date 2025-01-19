#include "VulkanShader.h"

#include <fstream>
#include <vector>


#include "Core/Logger.h"

#include "Utils.h"

VulkanShader::VulkanShader()
{
}

VulkanShader::~VulkanShader()
{
	vkDestroyShaderModule(mDevice, mShader, nullptr);
}

void VulkanShader::Initialize(VkDevice iDevice, const std::string& iFilePath)
{
	mDevice = iDevice;

	// open the file. With cursor at the end
	std::ifstream file(iFilePath, std::ios::ate | std::ios::binary);

	std::string wBaseFilename = iFilePath.substr(iFilePath.find_last_of("/\\") + 1);

	if (!file.is_open()) 
	{
		Logger::Get().mLogger->error("Failed to load shader file {:s}", wBaseFilename);
	}

	// find what the size of the file is by looking up the location of the cursor
	// because the cursor is at the end, it gives the size directly in bytes
	size_t fileSize = (size_t)file.tellg();

	// spirv expects the buffer to be on uint32, so make sure to reserve a int
	// vector big enough for the entire file
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

	// put file cursor at beginning
	file.seekg(0);

	// load the entire file into the buffer
	file.read((char*)buffer.data(), fileSize);

	// now that the file is loaded into the buffer, we can close it
	file.close();

	// create a new shader module, using the buffer we loaded
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;

	// codeSize has to be in bytes, so multply the ints in the buffer by size of
	// int to know the real size of the buffer
	createInfo.codeSize = buffer.size() * sizeof(uint32_t);
	createInfo.pCode = buffer.data();

	// check that the creation goes well.
	VkResult wResult =  vkCreateShaderModule(iDevice, &createInfo, nullptr, &mShader);
	CHECK_VK_RESULT(wResult, "Shader Module Creation");

	std::string wStageStr;

	if (iFilePath.ends_with("vert.spv"))
	{
		wStageStr = "Vertex";
		mStage = EStage::Vertex;
	}
	else if (iFilePath.ends_with("frag.spv"))
	{
		wStageStr = "Fragment";
		mStage = EStage::Fragment;
	}
	else if (iFilePath.ends_with("comp.spv"))
	{
		wStageStr = "Compute";
		mStage = EStage::Compute;
	}
	else
	{
		wStageStr = "Unknown";
		mStage = EStage::Unknown;
	}

	Logger::Get().mLogger->info("{0:s} Shader Module for {1:s} is created.", wStageStr, wBaseFilename);
}
