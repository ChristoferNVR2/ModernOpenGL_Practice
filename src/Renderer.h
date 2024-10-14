//
// Created by chrisvega on 10/11/24.
//

#pragma once

#include <GL/glew.h>
#include <csignal>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

// Platform-specific debug break
#ifdef __linux__
    #define debugBreak() raise(SIGTRAP)  // Linux equivalent for debug break
#elif _WIN32
    #define debugBreak() __debugbreak()  // Windows debug break
#endif

#define ASSERT(x) if (!(x)) debugBreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};