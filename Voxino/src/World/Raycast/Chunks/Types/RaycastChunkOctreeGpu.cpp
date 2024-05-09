#include "RaycastChunkOctreeGpu.h"
#include "Utils/RGBA.h"
#include "pch.h"

namespace Voxino::Raycast
{

RaycastChunkOctreeGpu::RaycastChunkOctreeGpu(const Block::Coordinate& blockPosition,
                                             const TexturePackArray& texturePack)
    : Chunk(blockPosition, texturePack)
{
    fillData();
}
RaycastChunkOctreeGpu::RaycastChunkOctreeGpu(Block::Coordinate blockPosition,
                                             const TexturePackArray& texturePack,
                                             ChunkContainerBase& parent)
    : Chunk(blockPosition, texturePack, parent)
{
    fillData();
}

int RaycastChunkOctreeGpu::numberOfVertices()
{
    return 0;
}

unsigned long RaycastChunkOctreeGpu::memorySize()
{
    return mOctree.allocatedBytes();
}

void RaycastChunkOctreeGpu::fillData()
{
    MEASURE_SCOPE;
    // sf::Clock buildingTime;
    mOctree.fillData(*mChunkOfBlocks);
    // auto buildingTimeElapsed = buildingTime.getElapsedTime().asMicroseconds();
    // spdlog::info("Building took: {} us, {} ms, {} s", buildingTimeElapsed,
    //              buildingTimeElapsed / 1000.f, buildingTimeElapsed / 1000000.f);
}


void RaycastChunkOctreeGpu::draw(const Renderer& renderer, const Shader& shader,
                                 const Camera& camera) const
{
    shader.bind();
    shader.setUniform("u_VoxelWorldPosition",
                      glm::vec3(mChunkPosition.x, mChunkPosition.y, mChunkPosition.z));
    mOctree.draw(renderer, shader, camera);
}

void RaycastChunkOctreeGpu::update(const float& deltaTime)
{
    mOctree.updateCounters();
    auto lastIterations = static_cast<int64_t>(mOctree.lastNumberOfRayIterations());
    TracyPlot("Ray Count", lastIterations);
}

void RaycastChunkOctreeGpu::updateImGui()
{
    ImGui::Begin("Ray Iterations");
    int lastIterations = mOctree.lastNumberOfRayIterations();
    ImGui::Text("Rays: %d", lastIterations);
    ImGui::End();
}

}// namespace Voxino::Raycast