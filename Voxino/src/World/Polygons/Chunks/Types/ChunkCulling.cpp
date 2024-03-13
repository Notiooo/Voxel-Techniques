#include "ChunkCulling.h"
#include "pch.h"

#include <Resources/TexturePack.h>

namespace Voxino
{


ChunkCulling::ChunkCulling(const Block::Coordinate& blockPosition, const TexturePack& texturePack,
                           ChunkContainer& parent)
    : Chunk(blockPosition, texturePack, parent)
    , mTexturePack(static_cast<const TexturePackAtlas&>(texturePack))
    , mTerrainMeshBuilder(mChunkPosition)
    , mFluidMeshBuilder(mChunkPosition)
    , mFloralMeshBuilder(mChunkPosition)
{
    initializeChunk();
}

ChunkCulling::ChunkCulling(const Block::Coordinate& blockPosition, const TexturePack& texturePack)
    : Chunk(blockPosition, texturePack)
    , mTexturePack(static_cast<const TexturePackAtlas&>(texturePack))
    , mTerrainMeshBuilder(blockPosition)
    , mFluidMeshBuilder(blockPosition)
    , mFloralMeshBuilder(blockPosition)
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

void ChunkCulling::rebuildMesh()
{
    MEASURE_SCOPE;
    mTerrainMeshBuilder.resetMesh();
    mFluidMeshBuilder.resetMesh();
    mFloralMeshBuilder.resetMesh();
    prepareMesh();
}

void ChunkCulling::updateMesh()
{
    MEASURE_SCOPE;
    if (!mTerrainModel)
    {
        mTerrainModel = std::make_unique<Model3D>();
    }
    mTerrainModel->setMesh(mTerrainMeshBuilder.mesh3D());

    if (!mFluidModel)
    {
        mFluidModel = std::make_unique<Model3D>();
    }
    mFluidModel->setMesh(mFluidMeshBuilder.mesh3D());

    if (!mFloralModel)
    {
        mFloralModel = std::make_unique<Model3D>();
    }
    mFloralModel->setMesh(mFloralMeshBuilder.mesh3D());
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
                    mFluidMeshBuilder.addQuad(
                        static_cast<Block::Face>(i),
                        mTexturePack.normalizedCoordinates(
                            block.blockTextureId(static_cast<Block::Face>(i))),
                        pos);
                }
            }
            else if (block.isFloral())
            {
                mFloralMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                           mTexturePack.normalizedCoordinates(
                                               block.blockTextureId(static_cast<Block::Face>(i))),
                                           pos);
            }
            else
            {
                mTerrainMeshBuilder.addQuad(static_cast<Block::Face>(i),
                                            mTexturePack.normalizedCoordinates(
                                                block.blockTextureId(static_cast<Block::Face>(i))),
                                            pos);
            }
        }
    }
}

}// namespace Voxino