#include "ChunkCullingGpu.h"
#include "pch.h"

#include <Resources/TexturePack.h>

namespace Voxino
{


ChunkCullingGpu::ChunkCullingGpu(const Block::Coordinate& blockPosition,
                                 const TexturePackArray& texturePack, ChunkContainer& parent)
    : ChunkArray(blockPosition, texturePack, parent)
{
    initializeChunk();
}

ChunkCullingGpu::ChunkCullingGpu(const Block::Coordinate& blockPosition,
                                 const TexturePackArray& texturePack)
    : ChunkArray(blockPosition, texturePack)
{
    initializeChunk();
}

void ChunkCullingGpu::initializeChunk()
{
    prepareMesh();
    updateMesh();
}

void ChunkCullingGpu::drawTerrain(const Renderer& renderer, const Shader& shader,
                                  const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    mTexturePack.bind(TexturePack::Spritesheet::Blocks);
    if (mTerrainModel)
    {
        mTerrainModel->draw(renderer, shader, camera, Renderer::DrawMode::Points);
    }
}

void ChunkCullingGpu::drawLiquids(const Renderer& renderer, const Shader& shader,
                                  const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    mTexturePack.bind(TexturePack::Spritesheet::Blocks);
    if (mFluidModel)
    {
        mFluidModel->draw(renderer, shader, camera, Renderer::DrawMode::Points);
    }
}

void ChunkCullingGpu::drawFlorals(const Renderer& renderer, const Shader& shader,
                                  const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    mTexturePack.bind(TexturePack::Spritesheet::Blocks);
    if (mFloralModel)
    {
        mFloralModel->draw(renderer, shader, camera, Renderer::DrawMode::Points);
    }
}

void ChunkCullingGpu::prepareMesh()
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

void ChunkCullingGpu::createBlockMesh(const Block::Coordinate& pos, const Block& block)
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
                    mFluidMeshBuilder.addPoint(static_cast<Block::Face>(i),
                                               block.blockTextureId(static_cast<Block::Face>(i)),
                                               pos);
                }
            }
            else if (block.isFloral())
            {
                mFloralMeshBuilder.addPoint(static_cast<Block::Face>(i),
                                            block.blockTextureId(static_cast<Block::Face>(i)), pos);
            }
            else
            {
                mTerrainMeshBuilder.addPoint(static_cast<Block::Face>(i),
                                             block.blockTextureId(static_cast<Block::Face>(i)),
                                             pos);
            }
        }
    }
}

}// namespace Voxino