#pragma once

#include "World/Chunks/Chunk.h"
#include "World/Raycast/Chunks/BrickgridGpu.h"
#include "World/Raycast/Chunks/Brickmap.h"
#include "World/Raycast/Chunks/VoxelsGpu.h"

namespace Voxino::Raycast
{

/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class RaycastChunkBrickmapGpu : public Chunk
{
public:
    static constexpr int BRICKS_PER_DIMENSION =
        ChunkBlocks::BLOCKS_PER_DIMENSION / Brickmap::BRICK_SIZE;
    static constexpr int BRICKS_PER_CHUNK =
        BRICKS_PER_DIMENSION * BRICKS_PER_DIMENSION * BRICKS_PER_DIMENSION;


    RaycastChunkBrickmapGpu(const Block::Coordinate& blockPosition,
                            const TexturePackArray& texturePack);

    /**
     * Returns the number of chunk vertices
     * @return Number of vertices
     */
    int numberOfVertices() override;

    /**
     * Returns the size in memory that the mesh occupies
     * @return The size in memory in bytes that the chunk occupies
     */
    int memorySize() override;

    /**
     * Draws this chunk to the game screen
     * \param renderer Renderer drawing the 3D game world onto the 2D screen
     * \param shader Shader with the help of which the object should be drawn
     * \param camera A camera in 3D space that looks at this object
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const override;

    /**
     * \brief Updates the state logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     */
    void update(const float& deltaTime) override;

    /**
     * \brief Updates the status/logic of the ImGui Debug Menu
     */
    void updateImGui() override;

private:
    void fillData();
    Brickmap& brickmap(int x, int y, int z);

private:
    BrickgridGpu mBrickgrid;
};

}// namespace Voxino::Raycast