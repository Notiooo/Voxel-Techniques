#pragma once

#include "Resources/TexturePackAtlas.h"
#include "World/Polygons/Chunks/Types/Chunk.h"
#include "World/Polygons/Meshes/Builders/ChunkAtlasMeshBuilder.h"

namespace Voxino
{

/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class ChunkCulling : public Chunk
{
public:
    ChunkCulling(const Block::Coordinate& blockPosition, const TexturePackArray& texturePack,
                 ChunkContainer& parent);
    ChunkCulling(const Block::Coordinate& blockPosition, const TexturePackArray& texturePack);
    /**
     * \brief Prepares/generates the mesh chunk, but does not replace it yet.
     */
    void prepareMesh() final;

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
}// namespace Voxino