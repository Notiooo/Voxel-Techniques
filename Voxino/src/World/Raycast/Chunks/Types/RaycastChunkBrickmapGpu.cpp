#include "RaycastChunkBrickmapGpu.h"
#include "Utils/RGBA.h"
#include "pch.h"

namespace Voxino::Raycast
{


RaycastChunkBrickmapGpu::RaycastChunkBrickmapGpu(const Block::Coordinate& blockPosition,
                                                 const TexturePackArray& texturePack)
    : Chunk(blockPosition, texturePack)
{
    fillData();
}
RaycastChunkBrickmapGpu::RaycastChunkBrickmapGpu(Block::Coordinate blockPosition,
                                                 const TexturePackArray& texturePack,
                                                 ChunkContainerBase& parent)
    : Chunk(blockPosition, texturePack, parent)
{
    fillData();
}

int RaycastChunkBrickmapGpu::numberOfVertices()
{
    return 0;
}

unsigned long RaycastChunkBrickmapGpu::memorySize()
{
    return mBrickgrid.allocatedBytes();
}

void RaycastChunkBrickmapGpu::fillData()
{
    MEASURE_SCOPE;
    // sf::Clock buildingTime;
    const int totalBricks = BRICKS_PER_DIMENSION * BRICKS_PER_DIMENSION * BRICKS_PER_DIMENSION;
    std::bitset<totalBricks> brickmapNeedsCreation;

    for (const auto& [position, block]: *mChunkOfBlocks)
    {

        // Calculate which brickmap this voxel belongs to
        int brickX = position.x / Brickmap::BRICK_SIZE;
        int brickY = position.y / Brickmap::BRICK_SIZE;
        int brickZ = position.z / Brickmap::BRICK_SIZE;
        int index = (brickZ * BrickgridGpu::GRID_SIZE * BrickgridGpu::GRID_SIZE) +
                    (brickY * BrickgridGpu::GRID_SIZE) + brickX;

        // Calculate index in the brick array
        int localX = position.x % Brickmap::BRICK_SIZE;
        int localY = position.y % Brickmap::BRICK_SIZE;
        int localZ = position.z % Brickmap::BRICK_SIZE;
        int localIndex = localZ * Brickmap::BRICK_SIZE * Brickmap::BRICK_SIZE +
                         localY * Brickmap::BRICK_SIZE + localX;


        // Only create Brickmap if there is a non-air block
        if (block.id() != BlockId::Air)
        {
            if (!mBrickgrid.getBrickmap(brickX, brickY, brickZ) && !brickmapNeedsCreation[index])
            {
                mBrickgrid.setBrickmap(brickX, brickY, brickZ, std::make_unique<Brickmap>());
                brickmapNeedsCreation.set(index);
            }

            // Get the brickmap if it exists
            auto& brick = mBrickgrid.getBrickmap(brickX, brickY, brickZ);
            brick->textureIds[localIndex] = block.toRGBA();// Convert block data to RGBA
        }
    }

    // Update the Brickgrid to handle new brickmaps
    mBrickgrid.update();
    // auto buildingTimeElapsed = buildingTime.getElapsedTime().asMicroseconds();
    // spdlog::info("Building took: {} us, {} ms, {} s", buildingTimeElapsed,
    //              buildingTimeElapsed / 1000.f, buildingTimeElapsed / 1000000.f);
}


void RaycastChunkBrickmapGpu::draw(const Renderer& renderer, const Shader& shader,
                                   const Camera& camera) const
{
    shader.bind();
    shader.setUniform("u_VoxelWorldPosition",
                      glm::vec3(mChunkPosition.x, mChunkPosition.y, mChunkPosition.z));
    mBrickgrid.draw(renderer, shader, camera);
}

void RaycastChunkBrickmapGpu::update(const float& deltaTime)
{
    mBrickgrid.updateCounters();
    auto lastIterations = static_cast<int64_t>(mBrickgrid.lastNumberOfRayIterations());
    TracyPlot("Ray Count", lastIterations);
}

void RaycastChunkBrickmapGpu::updateImGui()
{
    ImGui::Begin("Ray Iterations");
    int lastIterations = mBrickgrid.lastNumberOfRayIterations();
    ImGui::Text("Rays: %d", lastIterations);
    ImGui::End();
}

}// namespace Voxino::Raycast