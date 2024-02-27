#pragma once

#include "Renderer/Core/Buffers/Buffer.h"
#include "Renderer/Core/Buffers/VertexBuffer.h"

namespace Voxino
{
class BufferLayout;

class VertexArray : public Buffer
{
public:
    VertexArray();
    VertexArray(const VertexBuffer& vb, const BufferLayout& layout);
    VertexArray(const VertexBuffer&) = delete;
    VertexArray(VertexArray&&) noexcept = default;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray& operator=(VertexArray&&) noexcept = default;

    ~VertexArray() override;

    /**
     * Binds a vertex array object
     */
    void bind() const override;

    /**
     * Unbinds a vertex array object
     */
    void unbind() const override;

    /**
     * \brief A function that assigns VertexBuffer it's according BufferLayout.
     *
     * For example, a VertexBuffer stores position information (3x float)
     * and another one stores texture information (2x float). In this case,
     * BufferLayout informs the following:
     *
     * layout.push<float>(3);
     * layout.push<float>(2);
     *
     * \param vb VertexBuffer
     * \param layout Layout corresponding to the contents of the vector.
     */
    void setBuffer(const VertexBuffer& vb, const BufferLayout& layout);
};
}// namespace Voxino