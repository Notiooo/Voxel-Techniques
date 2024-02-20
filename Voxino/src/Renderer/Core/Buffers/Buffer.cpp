#include "Renderer/Core/Buffers/Buffer.h"
#include "pch.h"

namespace Voxino
{
Buffer::Buffer(Buffer&& rhs) noexcept
{
    mBufferId = rhs.mBufferId;
    rhs.mBufferId = 0;
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept
{
    mBufferId = rhs.mBufferId;
    rhs.mBufferId = 0;
    return *this;
}
}// namespace Voxino