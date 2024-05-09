#pragma once

#include "Renderer/Core/Buffers/AtomicCounter.h"
#include "Renderer/Renderer.h"
#include "World/Chunks/ChunkBlocks.h"
#include "World/Raycast/Chunks/Brickmap.h"
#include "World/Raycast/Chunks/VoxelsGpu.h"

#include <bitset>
#include <utility>

namespace Voxino::Raycast
{


// TODO: Każdy Brickmap powinien być odpowiednio przesunięty względem chunka.
// W tym momencie wszystkieś na sobie.
class Brickgrid
{
public:
    static constexpr int GRID_SIZE = ChunkBlocks::BLOCKS_PER_DIMENSION / Brickmap::BRICK_SIZE;
    static constexpr int GRID_VOLUME = GRID_SIZE * GRID_SIZE * GRID_SIZE;

    Brickgrid(Block::Coordinate position)
        : mGrid()
        , mPosition(std::move(position))
    {
    }

    void fillData(ChunkBlocks& chunkBlocks)
    {
        MEASURE_SCOPE;

        for (const auto& [position, block]: chunkBlocks)
        {
            // Calculate which brickmap this voxel belongs to
            int brickX = position.x / Brickmap::BRICK_SIZE;
            int brickY = position.y / Brickmap::BRICK_SIZE;
            int brickZ = position.z / Brickmap::BRICK_SIZE;
            int i = index(brickX, brickY, brickZ);

            // Calculate index in the brick array
            int localX = position.x % Brickmap::BRICK_SIZE;
            int localY = position.y % Brickmap::BRICK_SIZE;
            int localZ = position.z % Brickmap::BRICK_SIZE;

            if (block.id() != BlockId::Air)
            {
                if (not mGrid[i])
                {
                    mGrid[i] = std::make_unique<VoxelsGpu>(
                        Brickmap::BRICK_SIZE, Brickmap::BRICK_SIZE, Brickmap::BRICK_SIZE);
                }

                auto& brick = mGrid[i];
                brick->updateSingleBlock({localX, localY, localZ}, block.toRGBA().toArray());
            }
        }
    }

    /**
     * \brief Returns the number of ray iterations in the last frame.
     * \return The number of ray iterations in the last frame.
     */
    int lastNumberOfRayIterations() const;

    /**
     * \brief Updates the atomic counter and resets it.
     */
    void updateCounters();

    /**
     * \brief Returns the index of a brickmap in the grid.
     * \param x The x coordinate.
     * \param y The y coordinate.
     * \param z The z coordinate.
     * \return The index of the brickmap.
     */
    static size_t index(size_t x, size_t y, size_t z);

    /**
     * \brief Returns the brickmap at the specified coordinates.
     * \param x The x coordinate.
     * \param y The y coordinate.
     * \param z The z coordinate.
     * \return The brickmap at the specified coordinates.
     */
    const VoxelsGpu& brickmap(size_t x, size_t y, size_t z);

    /**
     * \brief Removes the brickmap at the specified coordinates.
     * \param x The x coordinate.
     * \param y The y coordinate.
     * \param z The z coordinate.
     */
    void remove(size_t x, size_t y, size_t z);

    /**
     * \brief Updates the Brickgrid.
     */
    void update();

    /**
     * \brief Draws the 3D texture to the screen.
     * \param renderer The renderer to draw with.
     * \param shader The shader to draw with.
     * \param camera The camera to draw with.
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const;

private:
    std::array<std::unique_ptr<VoxelsGpu>, GRID_VOLUME> mGrid;
    Block::Coordinate mPosition;
};

}// namespace Voxino::Raycast