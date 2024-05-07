#include "AtomicCounter.h"
#include "pch.h"


namespace Voxino
{

AtomicCounter::AtomicCounter()
    : Buffer()
{
    glGenBuffers(1, &mBufferId);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, mBufferId);
    GLuint initialValue = 0;
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &initialValue, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, mBufferId);
}

AtomicCounter::~AtomicCounter()
{
    glDeleteBuffers(1, &mBufferId);
}

void AtomicCounter::reset() const
{
    GLuint zero = 0;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, mBufferId);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

GLuint AtomicCounter::read() const
{
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, mBufferId);
    GLuint* ptr =
        (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT);
    GLuint value = 0;
    if (ptr)
    {
        value = *ptr;
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    }
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    return value;
}

void AtomicCounter::increment(GLuint incrementValue = 1)
{
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, mBufferId);
    GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
                                            GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    if (ptr)
    {
        *ptr += incrementValue;
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    }
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void AtomicCounter::decrement(GLuint decrementValue = 1)
{
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, mBufferId);
    // Map the buffer to access the current counter value
    GLuint* ptr = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
                                            GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    if (ptr)
    {
        // Ensure we don't underflow the counter
        *ptr = (*ptr > decrementValue) ? (*ptr - decrementValue) : 0;
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    }
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void AtomicCounter::bind() const
{
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, mBufferId);
}

void AtomicCounter::unbind() const
{
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, 0);
}


}// namespace Voxino