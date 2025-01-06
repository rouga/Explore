# Declare the library
add_library(Engine STATIC src/Engine.cpp
						  src/Engine.h
						  src/Window.cpp
						  src/Window.h
						  src/Renderer.cpp
						  src/Renderer.h
						  src/StaticMesh.cpp
						  src/StaticMesh.h
						  src/Input.cpp
						  src/Input.h
						  src/Camera.cpp
						  src/Camera.h
						  src/graphics/VulkanInstance.cpp
						  src/graphics/VulkanInstance.h
						  src/graphics/RenderContext.cpp
						  src/graphics/RenderContext.h
						  src/graphics/Utils.h 
						  src/graphics/Utils.cpp
						  src/graphics/VulkanDebugCallback.h 
						  src/graphics/VulkanDebugCallback.cpp
						  src/graphics/VulkanSwapchain.h 
						  src/graphics/VulkanSwapchain.cpp
						  src/graphics/VulkanPhysicalDevice.h 
						  src/graphics/VulkanPhysicalDevice.cpp
						  src/graphics/VulkanLogicalDevice.h
						  src/graphics/VulkanLogicalDevice.cpp
						  src/graphics/VulkanSemaphore.h
						  src/graphics/VulkanSemaphore.cpp
						  src/graphics/VulkanFence.h
						  src/graphics/VulkanFence.cpp
						  src/graphics/VulkanCommandBuffer.h
						  src/graphics/VulkanCommandBuffer.cpp
						  src/graphics/VulkanQueue.h
						  src/graphics/VulkanQueue.cpp
						  src/graphics/VulkanShader.h
						  src/graphics/VulkanShader.cpp
						  src/graphics/VulkanGraphicsPipeline.h
						  src/graphics/VulkanGraphicsPipeline.cpp
						  src/graphics/VulkanRenderPass.h
						  src/graphics/VulkanRenderPass.cpp
						  src/graphics/VulkanGPUBuffer.h
						  src/graphics/VulkanGPUBuffer.cpp
						  src/graphics/VulkanDescriptorPool.h
						  src/graphics/VulkanDescriptorPool.cpp
						  src/graphics/VulkanDescriptorSet.h
						  src/graphics/VulkanDescriptorSet.cpp
						  src/graphics/DescriptorSetManager.h
						  src/graphics/DescriptorSetManager.cpp
						  src/graphics/PipelineLayoutManager.h
						  src/graphics/PipelineLayoutManager.cpp
						  src/graphics/VulkanImage.h
						  src/graphics/VulkanImage.cpp
						  src/RenderPass.h
						  src/RenderPass.cpp
						  src/MainPass.h
						  src/MainPass.cpp)

# Include Vulkan
find_package(Vulkan REQUIRED)

# Include directories for this library
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
										 ${Vulkan_INCLUDE_DIRS}
										 ${CMAKE_SOURCE_DIR}/extern)

# Link Vulkan to the engine
target_link_libraries(Engine PUBLIC Vulkan::Vulkan glfw)