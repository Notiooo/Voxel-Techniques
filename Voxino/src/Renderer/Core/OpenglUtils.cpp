#include "Renderer/Core/OpenglUtils.h"
#include "pch.h"

namespace Voxino
{
void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
    {
        ;
    }
}

bool GLLogCall(const char* function, const char* file, int line)
{
    if (const GLenum error = glGetError())
    {
        spdlog::error("[OpenGL Error] (0x{:X}): {} {} at line: {}", error, function, file, line);
        return false;
    }
    return true;
}
}// namespace Voxino