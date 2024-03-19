#include "ChunkNaive.h"
#include "pch.h"

namespace Voxino
{


ChunkNaive::ChunkNaive(const Block::Coordinate& blockPosition, const TexturePackArray& texturePack,
                       ChunkContainer& parent)
    : Chunk(blockPosition, texturePack, parent)
{
    initializeChunk();
}

ChunkNaive::ChunkNaive(const Block::Coordinate& blockPosition, const TexturePackArray& texturePack)
    : Chunk(blockPosition, texturePack)
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
        if (block.id() == BlockId::Water)
        {
            mFluidMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                      block.blockTextureId(static_cast<Block::Face>(i)), pos);
        }
        else if (block.isFloral())
        {
            mFloralMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                       block.blockTextureId(static_cast<Block::Face>(i)), pos);
        }
        else
        {
            mTerrainMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                        block.blockTextureId(static_cast<Block::Face>(i)), pos);
        }
    }
}

void ChunkNaive::initializeChunk()
{
    prepareMesh();
    updateMesh();
}

}// namespace Voxino