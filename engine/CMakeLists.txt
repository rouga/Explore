# Declare the library
add_library(Engine STATIC src/Engine.cpp
						  src/Engine.h
						  src/Window.cpp
						  src/Window.h
						  src/graphics/VulkanInstance.cpp
						  src/graphics/VulkanInstance.h
						  src/graphics/RenderContext.cpp
						  src/graphics/RenderContext.h
						  src/graphics/Utils.h 
						  src/graphics/Utils.cpp
						  src/graphics/VulkanDebugCallback.h 
						  src/graphics/VulkanDebugCallback.cpp)

# Include Vulkan
find_package(Vulkan REQUIRED)

# Include directories for this library
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
										 ${Vulkan_INCLUDE_DIRS}
										 ${CMAKE_SOURCE_DIR}/extern)

# Link Vulkan to the engine
target_link_libraries(Engine PUBLIC Vulkan::Vulkan glfw)