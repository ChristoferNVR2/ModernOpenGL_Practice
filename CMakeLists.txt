cmake_minimum_required(VERSION 3.29)
project(ModernOpenGL)

set(CMAKE_CXX_STANDARD 20)

# Find the GLFW package
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

# Include GLFW header files
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})

add_executable(ModernOpenGL main.cpp)

# Link GLFW
target_link_libraries(ModernOpenGL PRIVATE glfw OpenGL::GL GLEW::GLEW)