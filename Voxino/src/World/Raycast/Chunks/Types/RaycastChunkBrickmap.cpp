#include "RaycastChunkBrickmap.h"
#include "Utils/RGBA.h"
#include "pch.h"

namespace Voxino::Raycast
{

RaycastChunkBrickmap::RaycastChunkBrickmap(const Block::Coordinate& blockPosition,
                                           const TexturePackArray& texturePack)
    : Chunk(blockPosition, texturePack)
    , mBrickgrid(mChunkPosition)
{
    fillData();
}
RaycastChunkBrickmap::RaycastChunkBrickmap(Block::Coordinate blockPosition,
                                           const TexturePackArray& texturePack,
                                           ChunkContainerBase& parent)
    : Chunk(blockPosition, texturePack, parent)
    , mBrickgrid(mChunkPosition)
{
    fillData();
}

int RaycastChunkBrickmap::numberOfVertices()
{
    return 0;
}

unsigned long RaycastChunkBrickmap::memorySize()
{
    return 0;
}

void RaycastChunkBrickmap::fillData()
{
    MEASURE_SCOPE;
    mBrickgrid.fillData(*mChunkOfBlocks);
}


void RaycastChunkBrickmap::draw(const Renderer& renderer, const Shader& shader,
                                const Camera& camera) const
{
    mBrickgrid.draw(renderer, shader, camera);
}

void RaycastChunkBrickmap::update(const float& deltaTime)
{
    mBrickgrid.updateCounters();
    auto lastIterations = static_cast<int64_t>(mBrickgrid.lastNumberOfRayIterations());
    TracyPlot("Ray Count", lastIterations);
}

void RaycastChunkBrickmap::updateImGui()
{
    ImGui::Begin("Ray Iterations");
    int lastIterations = mBrickgrid.lastNumberOfRayIterations();
    ImGui::Text("Rays: %d", lastIterations);
    ImGui::End();
}

}// namespace Voxino::Raycast