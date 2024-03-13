#include "CoordinateBase.h"
#include "pch.h"

namespace Voxino
{

CoordinateBase::CoordinateBase(IntegerUnit x, IntegerUnit y, IntegerUnit z)
    : mBlockCoordinates({x, y, z})
{
}

CoordinateBase::CoordinateBase(sf::Vector3i blockCoordinates)
    : mBlockCoordinates({blockCoordinates.x, blockCoordinates.y, blockCoordinates.z})
{
}

CoordinateBase::CoordinateBase(glm::ivec3 blockCoordinates)
    : mBlockCoordinates(std::move(blockCoordinates))
{
}

CoordinateBase::CoordinateBase(CoordinateBase&& rhs) noexcept
    : mBlockCoordinates(rhs.mBlockCoordinates)
    , x(mBlockCoordinates.x)
    , y(mBlockCoordinates.y)
    , z(mBlockCoordinates.z)
{
}

CoordinateBase::CoordinateBase(const CoordinateBase& rhs) noexcept
    : mBlockCoordinates(rhs.mBlockCoordinates)
    , x(mBlockCoordinates.x)
    , y(mBlockCoordinates.y)
    , z(mBlockCoordinates.z)
{
}

CoordinateBase& CoordinateBase::operator=(const CoordinateBase& rhs)
{
    mBlockCoordinates = rhs.mBlockCoordinates;
    return *this;
}

CoordinateBase CoordinateBase::operator-(const CoordinateBase& rhs) const
{
    return mBlockCoordinates - rhs.mBlockCoordinates;
}

CoordinateBase CoordinateBase::operator+(const CoordinateBase& rhs) const
{
    return mBlockCoordinates + rhs.mBlockCoordinates;
}

bool CoordinateBase::operator==(const CoordinateBase& rhs) const
{
    return mBlockCoordinates == rhs.mBlockCoordinates;
}

bool CoordinateBase::operator!=(const CoordinateBase& rhs) const
{
    return !(operator==(rhs));
}

}// namespace Voxino