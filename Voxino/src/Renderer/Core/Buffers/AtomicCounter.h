#pragma once
#include "Renderer/Core/Buffers/Buffer.h"


namespace Voxino
{

/**
 * \brief An atomic counter buffer object is used to store a single unsigned integer value that can
 * be read, incremented, or decremented atomically.
 */
class AtomicCounter : public Buffer
{
public:
    /**
     * \brief Creates an atomic counter buffer object
     */
    AtomicCounter();

    /**
     * \brief Deletes the atomic counter buffer object
     */
    ~AtomicCounter() override;

    /**
     * \brief Resets the atomic counter buffer object to zero
     */
    void reset() const;

    /**
     * \brief Reads the value of the atomic counter buffer object
     * \return the value of the atomic counter buffer object
     */
    GLuint read() const;

    /**
     * \brief Increments the value of the atomic counter buffer object
     * \param incrementValue the value to increment by
     */
    void increment(GLuint incrementValue);

    /**
     * \brief Decrements the value of the atomic counter buffer object
     * \param decrementValue the value to decrement by
     */
    void decrement(GLuint decrementValue);

    /**
     * \brief Binds the atomic counter buffer object
     */
    void bind() const override;

    /**
     * \brief Unbinds the atomic counter buffer object
     */
    void unbind() const override;
};

}// namespace Voxino