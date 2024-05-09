#include "RaycastChunk.h"
#include "Utils/RGBA.h"
#include "pch.h"

namespace Voxino::Raycast
{

RaycastChunk::RaycastChunk(const Block::Coordinate& blockPosition,
                           const TexturePackArray& texturePack)
    : Chunk(blockPosition, texturePack)
    , mVoxels(ChunkBlocks::BLOCKS_PER_X_DIMENSION, ChunkBlocks::BLOCKS_PER_Y_DIMENSION,
              ChunkBlocks::BLOCKS_PER_Z_DIMENSION)
{
    fillData();
}

int RaycastChunk::numberOfVertices()
{
    return 0;
}

int RaycastChunk::memorySize()
{
    return 0;
}

void RaycastChunk::fillData()
{
    MEASURE_SCOPE;
    std::vector<RGBA> data;
    for (const auto& [position, block]: *mChunkOfBlocks)
    {
        if (block.id() == BlockId::Air)
        {
            data.push_back(RGBA{0, 0, 0, 0});
        }
        else
        {
            data.push_back(block.toRGBA());
        }
    }
    mVoxels.fill(data);
}

void RaycastChunk::removeLocalBlock(const Block::Coordinate& localCoordinates)
{
    Chunk::removeLocalBlock(localCoordinates);
    // mVoxels.updateSingleBlock()
}

void RaycastChunk::draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const
{
    mVoxels.draw(renderer, shader, camera);
}

void RaycastChunk::update(const float& deltaTime)
{
    mVoxels.updateCounters();
}
void RaycastChunk::updateImGui()
{
    ImGui::Begin("Ray Iterations");
    int lastIterations = mVoxels.lastNumberOfRayIterations();
    ImGui::Text("Rays: %d", lastIterations);
    ImGui::End();
}

bool RaycastChunk::tryToPlaceBlockInsideThisChunk(const BlockId& blockId,
                                                  const Block::Coordinate& localCoordinates,
                                                  std::vector<BlockId>& blocksThatMightBeOverplaced)
{
    if (Chunk::tryToPlaceBlockInsideThisChunk(blockId, localCoordinates,
                                              blocksThatMightBeOverplaced))
    {
        auto& block = mChunkOfBlocks->block(localCoordinates);
        mVoxels.updateSingleBlock({localCoordinates.x, localCoordinates.y, localCoordinates.z},
                                  block.toRGBA().toArray());
        return true;
    }
    return false;
}

}// namespace Voxino::Raycast