
# Include Vulkan
find_package(Vulkan REQUIRED)

add_library(imgui STATIC
    imgui.cpp
    imgui_draw.cpp
    imgui_demo.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
				  	${CMAKE_SOURCE_DIR}/extern/GLFW/include)

target_link_libraries(imgui PRIVATE Vulkan::Vulkan)