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

RaycastChunk::RaycastChunk(Block::Coordinate blockPosition, const TexturePackArray& texturePack,
                           ChunkContainerBase& parent)
    : Chunk(blockPosition, texturePack, parent)
    , mVoxels(ChunkBlocks::BLOCKS_PER_X_DIMENSION, ChunkBlocks::BLOCKS_PER_Y_DIMENSION,
              ChunkBlocks::BLOCKS_PER_Z_DIMENSION)
{
    fillData();
}

int RaycastChunk::numberOfVertices()
{
    return 0;
}

unsigned long RaycastChunk::memorySize()
{
    return mVoxels.allocatedBytes();
}

unsigned long RaycastChunk::lastNumberOfRayIterations() const
{
    return mVoxels.lastNumberOfRayIterations();
}

void RaycastChunk::fillData()
{
    MEASURE_SCOPE;
    // sf::Clock buildingTime;
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
    // auto buildingTimeElapsed = buildingTime.getElapsedTime().asMicroseconds();
    // spdlog::info("Building took: {} us, {} ms, {} s", buildingTimeElapsed,
    //              buildingTimeElapsed / 1000.f, buildingTimeElapsed / 1000000.f);
}

void RaycastChunk::removeLocalBlock(const Block::Coordinate& localCoordinates)
{
    Chunk::removeLocalBlock(localCoordinates);
    // mVoxels.updateSingleBlock()
}

void RaycastChunk::draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const
{
    shader.bind();
    shader.setUniform("u_VoxelWorldPosition",
                      glm::vec3(mChunkPosition.x, mChunkPosition.y, mChunkPosition.z));
    mVoxels.draw(renderer, shader, camera);
}

void RaycastChunk::update(const float& deltaTime)
{
    mVoxels.updateCounters();
    auto lastIterations = static_cast<int64_t>(mVoxels.lastNumberOfRayIterations());
    TracyPlot("Ray Count", lastIterations);
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