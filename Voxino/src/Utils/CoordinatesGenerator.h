#pragma once
#include <World/Chunks/ChunkContainerBase.h>
#include <vector>

namespace Voxino
{
inline std::vector<glm::vec3> generateCoordinatesAround3D(const glm::vec3& center, int radius)
{
    std::vector<glm::vec3> coordinates;

    auto cx = center.x;
    auto cy = center.y;
    auto cz = center.z;

    for (int r = 1; r <= radius; ++r)
    {
        for (int dx = -r; dx <= r; ++dx)
        {
            for (int dy = -r; dy <= r; ++dy)
            {
                for (int dz = -r; dz <= r; ++dz)
                {
                    if (abs(dx) == r || abs(dy) == r || abs(dz) == r)
                    {
                        coordinates.emplace_back(cx + dx, cy + dy, cz + dz);
                    }
                }
            }
        }
    }

    return coordinates;
}

inline std::vector<glm::vec3> generateLimitedCoordinatesAround3D(
    const glm::vec3& center, int radius, int yLimit = ChunkContainerBase::MAX_CHUNKS_IN_HEIGHT)
{
    std::vector<glm::vec3> coordinates;

    auto cx = center.x;
    auto cy = center.y;
    auto cz = center.z;

    for (int r = 1; r <= radius; ++r)
    {
        for (int dx = -r; dx <= r; ++dx)
        {
            for (int dy = -r; dy <= r; ++dy)
            {
                for (int dz = -r; dz <= r; ++dz)
                {
                    if ((abs(dx) == r || abs(dy) == r || abs(dz) == r) && (cy + dy >= 0) &&
                        (cy + dy <= yLimit))
                    {
                        coordinates.emplace_back(cx + dx, cy + dy, cz + dz);
                    }
                }
            }
        }
    }

    return coordinates;
}
}// namespace Voxino