#include "ChunkNaive.h"
#include "pch.h"

namespace Voxino
{


ChunkNaive::ChunkNaive(const Block::Coordinate& blockPosition, const TexturePack& texturePack,
                       ChunkContainer& parent)
    : Chunk(blockPosition, texturePack, parent)
    , mTexturePack(static_cast<const TexturePackAtlas&>(texturePack))
    , mTerrainMeshBuilder(mChunkPosition)
    , mFluidMeshBuilder(mChunkPosition)
    , mFloralMeshBuilder(mChunkPosition)
{
    initializeChunk();
}

ChunkNaive::ChunkNaive(const Block::Coordinate& blockPosition, const TexturePack& texturePack)
    : Chunk(blockPosition, texturePack)
    , mTexturePack(static_cast<const TexturePackAtlas&>(texturePack))
    , mTerrainMeshBuilder(mChunkPosition)
    , mFluidMeshBuilder(mChunkPosition)
    , mFloralMeshBuilder(mChunkPosition)
{
    initializeChunk();
}

void ChunkNaive::rebuildMesh()
{
    MEASURE_SCOPE;
    mTerrainMeshBuilder.resetMesh();
    mFluidMeshBuilder.resetMesh();
    mFloralMeshBuilder.resetMesh();
    prepareMesh();
}

void ChunkNaive::updateMesh()
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
                                      mTexturePack.normalizedCoordinates(
                                          block.blockTextureId(static_cast<Block::Face>(i))),
                                      pos);
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

void ChunkNaive::initializeChunk()
{
    prepareMesh();
    updateMesh();
}

}// namespace Voxino