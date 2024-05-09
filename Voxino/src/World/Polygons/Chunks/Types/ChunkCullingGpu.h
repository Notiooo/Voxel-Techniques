#pragma once
#include "World/Polygons/Chunks/Types/ChunkArray.h"
#include "World/Polygons/Meshes/Builders/ChunkArrayCullingGpuMeshBuilder.h"

namespace Voxino::Polygons
{

/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class ChunkCullingGpu : public ChunkArray<ChunkArrayCullingGpuMeshBuilder>
{
public:
    ChunkCullingGpu(const Block::Coordinate& blockPosition, const TexturePackArray& texturePack,
                    ChunkContainerBase& parent);// TODO
    ChunkCullingGpu(const Block::Coordinate& blockPosition, const TexturePackArray& texturePack);
    /**
     * \brief Prepares/generates the mesh chunk, but does not replace it yet.
     */
    void prepareMesh() final;

    /**
     * Draws this chunk terrain to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawTerrain(const Renderer& renderer, const Shader& shader,
                     const Camera& camera) const override;

private:
    /**
     * Initializes the chunk by preparing and updating its mesh.
     */
    void initializeChunk();

    /**
     * Creates the visual representation of a block at a specified position (only for those
     * faces that are needed).
     *
     * @param pos Position of the block within the chunk.
     * @param block The block to be visually represented.
     */
    void createBlockMesh(const Block::Coordinate& pos, const Block& block);
};
}// namespace Voxino::Polygons