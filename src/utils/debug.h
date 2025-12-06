#ifndef DEBUG_H
#define DEBUG_H

#include <GL/glew.h>
#include <iostream>

namespace Debug
{
    inline void glErrorCheck(const char* filename, int line) {
        GLenum errorNumber = glGetError();
        while (errorNumber != GL_NO_ERROR) {
            std::cerr << errorNumber << ", file: " << filename << ", line: " << line << std::endl;

            errorNumber = glGetError();
        }
    }
}

#define glErrorCheck() glErrorCheck(__FILE__, __LINE__)

#endif // DEBUG_H
