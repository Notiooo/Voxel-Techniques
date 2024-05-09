#include "ChunkNaive.h"
#include "World/Chunks/ChunkBlocks.h"
#include "pch.h"

namespace Voxino::Polygons
{


ChunkNaive::ChunkNaive(const Block::Coordinate& blockPosition, const TexturePackArray& texturePack,
                       ChunkContainerBase& parent)
    : ChunkArray(blockPosition, texturePack, parent)
{
    initializeChunk();
}

ChunkNaive::ChunkNaive(const Block::Coordinate& blockPosition, const TexturePackArray& texturePack)
    : ChunkArray(blockPosition, texturePack)
{
    initializeChunk();
}

void ChunkNaive::prepareMesh()
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

void ChunkNaive::createBlockMesh(const Block::Coordinate& pos, const Block& block)
{
    for (auto i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
    {
        mTerrainMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                    block.blockTextureId(static_cast<Block::Face>(i)), pos);
    }
}

void ChunkNaive::initializeChunk()
{
    MEASURE_SCOPE;
    TracyMessageAuto("Initializing ChunkNaive");
    prepareMesh();
    updateMesh();
    TracyMessageAuto("End of ChunkNaive initialization");
}

}// namespace Voxino::Polygons