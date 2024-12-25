cmake_minimum_required(VERSION 3.0...3.25)
project(
    Explore # name of the project
    VERSION 0.1.0 # any version number
    LANGUAGES CXX C # programming languages used by the project
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Vulkan path (update as necessary for your system)
find_package(Vulkan REQUIRED)

add_subdirectory(extern/glfw)

# Add subdirectories
add_subdirectory(engine)
add_subdirectory(editor)