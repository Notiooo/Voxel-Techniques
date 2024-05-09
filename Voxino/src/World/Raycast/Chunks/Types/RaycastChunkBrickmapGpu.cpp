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

int RaycastChunkBrickmapGpu::numberOfVertices()
{
    return 0;
}

int RaycastChunkBrickmapGpu::memorySize()
{
    return 0;
}

void RaycastChunkBrickmapGpu::fillData()
{
    MEASURE_SCOPE;

    const int totalBricks = BRICKS_PER_DIMENSION * BRICKS_PER_DIMENSION * BRICKS_PER_DIMENSION;
    std::bitset<totalBricks> brickmapNeedsCreation;

    for (const auto& [position, block]: *mChunkOfBlocks)
    {
        // Calculate which brickmap this voxel belongs to
        int brickX = position.x / Brickmap::BRICK_SIZE;
        int brickY = position.y / Brickmap::BRICK_SIZE;
        int brickZ = position.z / Brickmap::BRICK_SIZE;
        int index = (brickZ * BRICKS_PER_DIMENSION * BRICKS_PER_DIMENSION) +
                    (brickY * BRICKS_PER_DIMENSION) + brickX;

        // Calculate index in the brick array
        int localX = position.x % Brickmap::BRICK_SIZE;
        int localY = position.y % Brickmap::BRICK_SIZE;
        int localZ = position.z % Brickmap::BRICK_SIZE;
        int localIndex = localX * Brickmap::BRICK_SIZE * Brickmap::BRICK_SIZE +
                         localY * Brickmap::BRICK_SIZE + localZ;

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
}


void RaycastChunkBrickmapGpu::draw(const Renderer& renderer, const Shader& shader,
                                   const Camera& camera) const
{
    mBrickgrid.draw(renderer, shader, camera);
}

void RaycastChunkBrickmapGpu::update(const float& deltaTime)
{
    mBrickgrid.updateCounters();
}

void RaycastChunkBrickmapGpu::updateImGui()
{
    ImGui::Begin("Ray Iterations");
    int lastIterations = mBrickgrid.lastNumberOfRayIterations();
    ImGui::Text("Rays: %d", lastIterations);
    ImGui::End();
}

}// namespace Voxino::Raycast