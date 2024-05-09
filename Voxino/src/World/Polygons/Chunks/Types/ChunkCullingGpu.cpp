#include "ChunkCullingGpu.h"
#include "Resources/TexturePackArray.h"
#include "World/Chunks/ChunkBlocks.h"
#include "pch.h"

namespace Voxino::Polygons
{


ChunkCullingGpu::ChunkCullingGpu(const Block::Coordinate& blockPosition,
                                 const TexturePackArray& texturePack, ChunkContainerBase& parent)
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
    MEASURE_SCOPE;
    TracyMessageAuto("Initializing ChunkCullingGpu");
    prepareMesh();
    updateMesh();
    TracyMessageAuto("End of ChunkCullingGpu initialization");
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
            mTerrainMeshBuilder.addPoint(static_cast<Block::Face>(i),
                                         block.blockTextureId(static_cast<Block::Face>(i)), pos);
        }
    }
}

}// namespace Voxino::Polygons