#include "Renderer/Core/VertexArray.h"
#include "pch.h"

#include "Renderer/Core/Buffers/BufferLayout.h"
#include "Renderer/Core/OpenglUtils.h"
namespace Voxino
{
VertexArray::VertexArray()
{
    GLCall(glGenVertexArrays(1, &mBufferId));
}

VertexArray::VertexArray(const VertexBuffer& vb, const BufferLayout& layout)
    : VertexArray()
{
    setBuffer(vb, layout);
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &mBufferId));
}

void VertexArray::bind() const
{
    GLCall(glBindVertexArray(mBufferId));
}

void VertexArray::unbind() const
{
    GLCall(glBindVertexArray(0));
}

void VertexArray::setBuffer(const VertexBuffer& vb, const BufferLayout& layout)
{
    bind();
    vb.bind();

    const auto& elements = layout.bufferElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        const auto& [type, count, normalized] = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, count, type, normalized, layout.stride(),
                                     reinterpret_cast<const void*>(offset)));
        offset += count * BufferElement::sizeOfGLType(type);
    }
}
}// namespace Voxino