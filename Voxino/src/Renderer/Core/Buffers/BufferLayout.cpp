#include "Renderer/Core/Buffers/BufferLayout.h"
#include "Renderer/Core/OpenglUtils.h"
#include "pch.h"

namespace Voxino
{
unsigned BufferElement::sizeOfGLType(const unsigned type)
{
    switch (type)
    {
        case GL_FLOAT:
        case GL_UNSIGNED_INT: return 4;
        case GL_UNSIGNED_BYTE: return 1;
        default: ASSERT(false); return 0;
    }
}

BufferLayout::BufferLayout()
    : mStride(0)
{
}

std::vector<BufferElement> BufferLayout::bufferElements() const
{
    return mBufferElements;
}

unsigned BufferLayout::stride() const
{
    return mStride;
}
}// namespace Voxino