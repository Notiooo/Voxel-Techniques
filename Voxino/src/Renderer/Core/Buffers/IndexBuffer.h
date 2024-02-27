#pragma once
#include "Renderer/Core/Buffers/Buffer.h"


namespace Voxino
{
/**
 * A higher level representation of OpenGL IndexBuffer which is an array of pointers into the vertex
 * buffer.
 */
class IndexBuffer : public Buffer
{
public:
    IndexBuffer();
    IndexBuffer(const unsigned int* data, unsigned int count);

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer(IndexBuffer&&) noexcept = default;

    IndexBuffer& operator=(const IndexBuffer&) = delete;
    IndexBuffer& operator=(IndexBuffer&&) noexcept = default;

    ~IndexBuffer() override;

    /**
     * Binds a buffer object to the specific (GL_ELEMENT_ARRAY_BUFFER) buffer binding point
     */
    void bind() const override;

    /**
     * Unbinds a buffer object (GL_ELEMENT_ARRAY_BUFFER)
     */
    void unbind() const override;

    /**
     * Sets the data with the specified number of ints and binds the buffer
     * @param data Array of unsigned ints containing buffer indexes
     * @param count Size of the Array
     */
    void setBuffer(const unsigned int* data, unsigned count);


    /**
     * \brief Sets the data from vector and binds the buffer
     * \tparam T Type of vector
     * \param vector Vector having linearly arranged data of type T in memory
     */
    template<typename T>
    void setBuffer(const std::vector<T>& vector);

    /**
     * Returns the number of elements in the index buffer
     * @return Size of the index buffer
     */
    [[nodiscard]] unsigned int size() const;

private:
    unsigned int mCount;
};

template<typename T>
void IndexBuffer::setBuffer(const std::vector<T>& vector)
{
    setBuffer(vector.data(), vector.size());
}
}// namespace Voxino