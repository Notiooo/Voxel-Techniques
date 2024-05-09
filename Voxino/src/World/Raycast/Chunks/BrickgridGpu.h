#pragma once

#include "Renderer/Core/Buffers/AtomicCounter.h"
#include "Renderer/Renderer.h"
#include "World/Chunks/ChunkBlocks.h"
#include "World/Raycast/Chunks/Brickmap.h"

#include <bitset>

namespace Voxino
{

/**
 * \brief A grid of brickmaps.
 */
class BrickgridGpu
{
public:
    static constexpr int GRID_SIZE = ChunkBlocks::BLOCKS_PER_DIMENSION / Brickmap::BRICK_SIZE;
    static constexpr int GRID_VOLUME = GRID_SIZE * GRID_SIZE * GRID_SIZE;
    BrickgridGpu();
    ~BrickgridGpu();

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
     * \brief Returns the occupancy bitset.
     * \return The occupancy bitset.
     */
    std::bitset<GRID_VOLUME> occupancyBitset();

    /**
     * \brief Returns the brickmap at the specified coordinates.
     * \param x The x coordinate.
     * \param y The y coordinate.
     * \param z The z coordinate.
     * \return The brickmap at the specified coordinates.
     */
    const std::unique_ptr<Brickmap>& getBrickmap(size_t x, size_t y, size_t z);

    /**
     * \brief Sets the brickmap at the specified coordinates.
     * \param x The x coordinate.
     * \param y The y coordinate.
     * \param z The z coordinate.
     * \param brickmap The brickmap to set.
     */
    void setBrickmap(size_t x, size_t y, size_t z, std::unique_ptr<Brickmap>&& brickmap);

    /**
     * \brief Removes the brickmap at the specified coordinates.
     * \param x The x coordinate.
     * \param y The y coordinate.
     * \param z The z coordinate.
     */
    void removeBrickmap(size_t x, size_t y, size_t z);

    /**
     * \brief Updates the BrickgridGpu.
     */
    void update();

    /**
     * \brief Draws the 3D texture to the screen.
     * \param renderer The renderer to draw with.
     * \param shader The shader to draw with.
     * \param camera The camera to draw with.
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const;

    auto allocatedBytes() const
    {
        return mAllocatedBytes;
    }

private:
    std::array<std::unique_ptr<Brickmap>, GRID_VOLUME> mGrid;
    std::bitset<GRID_VOLUME> mOccupancyBitset;
    GLuint mOccupancyBuffer;
    GLuint mOccupancyTex;
    GLuint mTextureArray;
    bool mNeedsBufferUpdate{false};
    int mLastNumberOfRayIterations{};
    AtomicCounter mAtomicCounter;
    bool mIsDataFilled{false};
    mutable bool mWasDataFiledLogPrinted{false};
    unsigned long long mAllocatedBytes{0};
};

}// namespace Voxino