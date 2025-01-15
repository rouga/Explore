#include "Utils.h"

#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

VkImageView CreateImageView(VkDevice iDevice, VkImage iImage, VkFormat iFormat, VkImageAspectFlags iAspect, VkImageViewType iViewType, uint32_t iLayerCount, uint32_t iMipCount)
{
	VkImageViewCreateInfo wImageViewCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = iImage,
		.viewType = iViewType,
		.format = iFormat,
		.components =
		{
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY,
		},
		.subresourceRange =
		{
			.aspectMask = iAspect,
			.baseMipLevel = 0,
			.levelCount = iMipCount,
			.baseArrayLayer = 0,
			.layerCount = iLayerCount,
		},
	};

	VkImageView wImageView = VK_NULL_HANDLE;
	VkResult wResult = vkCreateImageView(iDevice, &wImageViewCreateInfo, nullptr, &wImageView);
	CHECK_VK_RESULT(wResult, "Image View Creation");

	return wImageView;
}

const char* GetDebugSeverityStr(VkDebugUtilsMessageSeverityFlagBitsEXT iSeverity)
{
	switch (iSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		return "Verbose";
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		return "Info";
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		return "Warning";
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		return "Error";
	default:
		throw std::exception("Invalid Severity Code : %d\n", iSeverity);
	}
}

const char* GetDebugTypeStr(VkDebugUtilsMessageTypeFlagsEXT iType)
{
	switch (iType)
	{
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		return "General";
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		return "Validation";
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		return "Performance";
	case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
		return "Device Address Binding";
	default:
		throw std::exception("Invalid Type Code : %d\n", iType);
	}
}

uint32_t FindMemoryType(VkPhysicalDevice iPhysicalDevice, VkMemoryPropertyFlags iMemProps)
{
	VkPhysicalDeviceMemoryProperties wMemProperties;
	vkGetPhysicalDeviceMemoryProperties(iPhysicalDevice, &wMemProperties);

	for (uint32_t i = 0; i < wMemProperties.memoryTypeCount; i++)
	{
		if ((wMemProperties.memoryTypes[i].propertyFlags & iMemProps) == iMemProps)
		{
			return i;
		}
	}

	spdlog::error("Cannot Find Memory Type");
	exit(EXIT_FAILURE);
}

namespace CmdDebug
{
	PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT{ nullptr };
	PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT{ nullptr };
	PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT{ nullptr };

	void SetupDebugUtils(VkInstance instance)
	{
		vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT"));
		vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT"));
		vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT"));
	}

	void CmdBeginLabel(VkCommandBuffer iCmdBuffer, std::string iCaption, glm::vec4 iColor)
	{
		if (!vkCmdBeginDebugUtilsLabelEXT)
		{
			return;
		}
		VkDebugUtilsLabelEXT labelInfo{};
		labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		labelInfo.pLabelName = iCaption.c_str();
		memcpy(labelInfo.color, &iColor[0], sizeof(float) * 4);
		vkCmdBeginDebugUtilsLabelEXT(iCmdBuffer, &labelInfo);
	}

	void CmdEndLabel(VkCommandBuffer iCmdBuffer)
	{
		if (!vkCmdEndDebugUtilsLabelEXT) {
			return;
		}
		vkCmdEndDebugUtilsLabelEXT(iCmdBuffer);
	}
};
