# Declare the library
add_library(Engine STATIC src/Core/Engine.cpp
						  src/Core/Engine.h
						  src/Core/Window.cpp
						  src/Core/Window.h
						  src/Core/Input.cpp
						  src/Core/Input.h
						  src/Core/Timer.cpp
						  src/Core/Timer.h
						  src/Renderer/Renderer.cpp
						  src/Renderer/Renderer.h
						  src/Renderer/Viewport.h
						  src/Renderer/Viewport.cpp
						  src/Renderer/RenderPass.h
						  src/Renderer/RenderPass.cpp
						  src/Renderer/MainPass.h
						  src/Renderer/MainPass.cpp
						  src/Renderer/UIPass.h
						  src/Renderer/UIPass.cpp
						  src/Renderer/UIManager.cpp
						  src/Renderer/UIManager.h
						  src/Renderer/TextureManager.h
						  src/Renderer/TextureManager.cpp
						  src/Scene/StaticMesh.cpp
						  src/Scene/StaticMesh.h
						  src/Scene/Camera.cpp
						  src/Scene/Camera.h
						  src/Scene/Model.cpp
						  src/Scene/Model.h
						  src/Scene/Entity.h
						  src/Scene/Transformation.cpp
						  src/Scene/Transformation.h
						  src/Graphics/VulkanInstance.cpp
						  src/Graphics/VulkanInstance.h
						  src/Graphics/RenderContext.cpp
						  src/Graphics/RenderContext.h
						  src/Graphics/Utils.h 
						  src/Graphics/Utils.cpp
						  src/Graphics/VulkanDebugCallback.h 
						  src/Graphics/VulkanDebugCallback.cpp
						  src/Graphics/VulkanSwapchain.h 
						  src/Graphics/VulkanSwapchain.cpp
						  src/Graphics/VulkanPhysicalDevice.h 
						  src/Graphics/VulkanPhysicalDevice.cpp
						  src/Graphics/VulkanLogicalDevice.h
						  src/Graphics/VulkanLogicalDevice.cpp
						  src/Graphics/VulkanSemaphore.h
						  src/Graphics/VulkanSemaphore.cpp
						  src/Graphics/VulkanFence.h
						  src/Graphics/VulkanFence.cpp
						  src/Graphics/VulkanCommandBuffer.h
						  src/Graphics/VulkanCommandBuffer.cpp
						  src/Graphics/VulkanQueue.h
						  src/Graphics/VulkanQueue.cpp
						  src/Graphics/VulkanShader.h
						  src/Graphics/VulkanShader.cpp
						  src/Graphics/VulkanGraphicsPipeline.h
						  src/Graphics/VulkanGraphicsPipeline.cpp
						  src/Graphics/VulkanRenderPass.h
						  src/Graphics/VulkanRenderPass.cpp
						  src/Graphics/VulkanGPUBuffer.h
						  src/Graphics/VulkanGPUBuffer.cpp
						  src/Graphics/VulkanDescriptorPool.h
						  src/Graphics/VulkanDescriptorPool.cpp
						  src/Graphics/VulkanDescriptorSet.h
						  src/Graphics/VulkanDescriptorSet.cpp
						  src/Graphics/DescriptorSetManager.h
						  src/Graphics/DescriptorSetManager.cpp
						  src/Graphics/PipelineLayoutManager.h
						  src/Graphics/PipelineLayoutManager.cpp
						  src/Graphics/VulkanImage.h
						  src/Graphics/VulkanImage.cpp)

# Include Vulkan
find_package(Vulkan REQUIRED)

# Include directories for this library
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
										 ${Vulkan_INCLUDE_DIRS}
										 ${CMAKE_SOURCE_DIR}/extern
										 ${CMAKE_SOURCE_DIR}/extern/assimp-5.4.3/include
										 ${CMAKE_SOURCE_DIR}/extern/imgui)

# Link Vulkan to the engine
target_link_libraries(Engine PRIVATE Vulkan::Vulkan glfw assimp imgui)