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
include_directories(${CMAKE_SOURCE_DIR}/src/vendor)

add_executable(ModernOpenGL
        src/main.cpp
        src/Renderer.cpp
        src/Renderer.h
        src/VertexBuffer.cpp
        src/VertexBuffer.h
        src/IndexBuffer.cpp
        src/IndexBuffer.h
        src/VertexArray.cpp
        src/VertexArray.h
        src/VertexBufferLayout.cpp
        src/VertexBufferLayout.h
        src/Shader.cpp
        src/Shader.h
        src/vendor/stb_image/stb_image.h
        src/vendor/stb_image/stb_image.cpp
        src/Texture.cpp
        src/Texture.h
)

# Link GLFW
target_link_libraries(ModernOpenGL PRIVATE glfw OpenGL::GL GLEW::GLEW)