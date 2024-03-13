#pragma once

#include "World/Polygons/Chunks/ChunkBlocks.h"
#include <bitset>

namespace Voxino
{

template<unsigned X_DIMENSION, unsigned Y_DIMENSION, unsigned Z_DIMENSION>
class Bitset3D
{
public:
    static constexpr auto FIELDS_IN_BITSET = X_DIMENSION * Y_DIMENSION * Z_DIMENSION;

    template<typename T>
    inline bool test(const T& dimensions)
    {
        return mBlocks.test(calculateIndex(dimensions.x, dimensions.y, dimensions.z));
    }

    inline bool test(unsigned short x, unsigned short y, unsigned short z)
    {
        return mBlocks[calculateIndex(x, y, z)];
    }

    template<typename T>
    inline void set(const T& dimensions, bool value)
    {
        mBlocks.set(calculateIndex(dimensions.x, dimensions.y, dimensions.z), value);
    }

    inline void set(unsigned short x, unsigned short y, unsigned short z, bool value)
    {
        mBlocks.set(calculateIndex(x, y, z), value);
    }

private:
    inline unsigned calculateIndex(unsigned short x, unsigned short y, unsigned short z) const
    {
        return (z * Y_DIMENSION * X_DIMENSION) + (y * X_DIMENSION) + x;
    }

private:
    std::bitset<FIELDS_IN_BITSET> mBlocks{false};
};


}// namespace Voxino