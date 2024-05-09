#include "ChunkCulling.h"
#include "Resources/TexturePack.h"
#include "World/Chunks/ChunkBlocks.h"
#include "pch.h"

namespace Voxino::Polygons
{


ChunkCulling::ChunkCulling(const Block::Coordinate& blockPosition,
                           const TexturePackArray& texturePack, ChunkContainerBase& parent)
    : ChunkArray(blockPosition, texturePack, parent)
{
    initializeChunk();
}

ChunkCulling::ChunkCulling(const Block::Coordinate& blockPosition,
                           const TexturePackArray& texturePack)
    : ChunkArray(blockPosition, texturePack)
{
    initializeChunk();
}

void ChunkCulling::initializeChunk()
{
    MEASURE_SCOPE;
    TracyMessageAuto("Initializing ChunkCulling");
    prepareMesh();
    updateMesh();
    TracyMessageAuto("End of ChunkCulling initialization");
}

void ChunkCulling::prepareMesh()
{
    MEASURE_SCOPE;
    for (const auto& [position, block]: *mChunkOfBlocks)
    {
        if (block.id() == BlockId::Air)
        {
            continue;
        }

        createBlockMesh(position, block);
    }
}

void ChunkCulling::createBlockMesh(const Block::Coordinate& pos, const Block& block)
{
    for (auto i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
    {
        if (doesBlockFaceHasTransparentNeighbor(static_cast<Block::Face>(i), pos))
        {
            mTerrainMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                        block.blockTextureId(static_cast<Block::Face>(i)), pos);
        }
    }
}

}// namespace Voxino::Polygons