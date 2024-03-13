#pragma once

#include "Resources/TexturePackAtlas.h"
#include "World/Polygons/Chunks/Types/Chunk.h"

#include <World/Polygons/Meshes/Builders/ChunkAtlasMeshBuilder.h>

namespace Voxino
{
class TexturePackAtlas;

/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class ChunkNaive : public Chunk
{
public:
    ChunkNaive(const Block::Coordinate& blockPosition, const TexturePack& texturePack,
               ChunkContainer& parent);
    ChunkNaive(const Block::Coordinate& blockPosition, const TexturePack& texturePack);

    /**
     * \brief Prepares/generates the mesh chunk, but does not replace it yet.
     */
    void prepareMesh() final;

    /**
     * \brief Swaps the current chunk mesh with the latest, most recently generated one
     */
    void updateMesh() final;

    /**
     * It is rebuilding this mesh fresh. Very expensive operation
     */
    void rebuildMesh() final;

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

private:
    const TexturePackAtlas& mTexturePack;

    // TODO: This system should be changed to a better one. Consider distance.
    ChunkAtlasMeshBuilder mTerrainMeshBuilder;
    ChunkAtlasMeshBuilder mFluidMeshBuilder;
    ChunkAtlasMeshBuilder mFloralMeshBuilder;
};

}// namespace Voxino