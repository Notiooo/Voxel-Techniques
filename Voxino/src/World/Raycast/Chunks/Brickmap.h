#pragma once
#include "Utils/RGBA.h"


/**
 * @brief Represents a 3D texture map with cubic "bricks" composed of RGBA voxels.
 *
 * Each brick is a cube of 8x8x8 voxels (BRICK_SIZE) with individual RGBA values stored in
 * `textureIds`. A voxel is solid if its alpha (A) is non-zero. Textures are written in
 * R, G, B channels on higher and lower part of channels, separately.
 */
struct Brickmap
{
    static constexpr int BRICK_SIZE = 8;
    static constexpr int BRICK_VOLUME = BRICK_SIZE * BRICK_SIZE * BRICK_SIZE;

    std::array<RGBA, BRICK_VOLUME> textureIds;
};