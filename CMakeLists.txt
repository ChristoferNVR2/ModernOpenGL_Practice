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
        src/vendor/imgui/imconfig.h
        src/vendor/imgui/imgui.cpp
        src/vendor/imgui/imgui.h
        src/vendor/imgui/imgui_demo.cpp
        src/vendor/imgui/imgui_draw.cpp
        src/vendor/imgui/imgui_impl_glfw_gl3.cpp
        src/vendor/imgui/imgui_impl_glfw_gl3.h
        src/vendor/imgui/imgui_internal.h
        src/vendor/imgui/stb_rect_pack.h
        src/vendor/imgui/stb_textedit.h
        src/vendor/imgui/stb_truetype.h
        src/ObjLoader.cpp
        src/ObjLoader.h
)

# Link GLFW
target_link_libraries(ModernOpenGL PRIVATE glfw OpenGL::GL GLEW::GLEW)