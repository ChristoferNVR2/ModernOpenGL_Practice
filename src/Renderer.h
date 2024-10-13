//
// Created by chrisvega on 10/11/24.
//

#pragma once

#include <GL/glew.h>
#include <csignal>

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
