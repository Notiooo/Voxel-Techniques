#include "ChunkCulling.h"
#include "Resources/TexturePack.h"
#include "World/Chunks/ChunkBlocks.h"
#include "pch.h"

namespace Voxino::Polygons
{


// ChunkCulling::ChunkCulling(const Block::Coordinate& blockPosition,
//                            const TexturePackArray& texturePack, ChunkContainer& parent)
//     : ChunkArray(blockPosition, texturePack, parent)
// {
//     initializeChunk();
// } // TODO

ChunkCulling::ChunkCulling(const Block::Coordinate& blockPosition,
                           const TexturePackArray& texturePack)
    : ChunkArray(blockPosition, texturePack)
{
    initializeChunk();
}

void ChunkCulling::initializeChunk()
{
    prepareMesh();
    updateMesh();
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
            if (block.id() == BlockId::Water)
            {
                if (!doesBlockFaceHasGivenBlockNeighbour(static_cast<Block::Face>(i), pos,
                                                         BlockId::Water))
                {
                    mFluidMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                              block.blockTextureId(static_cast<Block::Face>(i)),
                                              pos);
                }
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
}

}// namespace Voxino::Polygons