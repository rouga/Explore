# Declare the editor executable
add_executable(Editor src/Editor.cpp
					  src/Editor.h
					  src/Main.cpp)

# Include Vulkan
find_package(Vulkan REQUIRED)

# Link the engine library to the editor
target_link_libraries(Editor PRIVATE Engine Vulkan::Vulkan)

# Include directories for engine
target_include_directories(Editor PRIVATE ${CMAKE_SOURCE_DIR}/engine/src
										  ${CMAKE_SOURCE_DIR}/extern/GLFW/include)

add_custom_command(TARGET Editor POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                           ${CMAKE_SOURCE_DIR}/shaders 
					        $<TARGET_FILE_DIR:Editor>/shaders)

add_custom_command(TARGET Editor POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                           ${CMAKE_SOURCE_DIR}/resources 
					        $<TARGET_FILE_DIR:Editor>/resources)

set_property(TARGET Editor  PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})