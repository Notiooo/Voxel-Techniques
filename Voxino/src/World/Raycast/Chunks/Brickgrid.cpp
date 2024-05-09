#include "Brickgrid.h"
#include "pch.h"

#include <bitset>
#include <numeric>


namespace Voxino::Raycast
{


int Brickgrid::lastNumberOfRayIterations() const
{
    return std::accumulate(mGrid.cbegin(), mGrid.cend(), 0,
                           [](int acc, const auto& brickmap)
                           {
                               return acc + (brickmap ? brickmap->lastNumberOfRayIterations() : 0);
                           });
}

void Brickgrid::updateCounters()
{
    for (auto& brickmap: mGrid)
    {
        if (brickmap)
        {
            brickmap->updateCounters();
        }
    }
}

size_t Brickgrid::index(size_t x, size_t y, size_t z)
{
    return x * GRID_SIZE * GRID_SIZE + y * GRID_SIZE + z;
}


const VoxelsGpu& Brickgrid::brickmap(size_t x, size_t y, size_t z)
{
    return *mGrid[index(x, y, z)];
}

void Brickgrid::remove(size_t x, size_t y, size_t z)
{
    auto gridIndex = index(x, y, z);
    mGrid[gridIndex].reset();
}

void Brickgrid::draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    for (size_t i = 0; i < GRID_VOLUME; ++i)
    {
        if (mGrid[i])
        {
            auto z = i % GRID_SIZE;
            auto y = (i / GRID_SIZE) % GRID_SIZE;
            auto x = i / (GRID_SIZE * GRID_SIZE);
            const auto position = glm::vec3(mPosition.x + x * Brickmap::BRICK_SIZE,
                                            mPosition.y + y * Brickmap::BRICK_SIZE,
                                            mPosition.z + z * Brickmap::BRICK_SIZE);
            shader.bind();
            shader.setUniform("u_VoxelWorldPosition", position);
            mGrid[i]->draw(renderer, shader, camera);
        }
    }
}

}// namespace Voxino::Raycast