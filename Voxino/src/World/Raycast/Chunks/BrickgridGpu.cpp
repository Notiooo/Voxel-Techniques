#include "BrickgridGpu.h"
#include "pch.h"


namespace Voxino
{

BrickgridGpu::BrickgridGpu()
    : mOccupancyBuffer(0)
    , mOccupancyTex(0)
    , mTextureArray(0)
{
    GLCall(glGenBuffers(1, &mOccupancyBuffer));


    std::vector<unsigned char> buffer(GRID_VOLUME, 0);
    GLCall(glBindBuffer(GL_TEXTURE_BUFFER, mOccupancyBuffer));
    GLCall(glBufferData(GL_TEXTURE_BUFFER, buffer.size() * sizeof(unsigned char), buffer.data(),
                        GL_STATIC_DRAW));

    GLCall(glGenTextures(1, &mOccupancyTex));
    GLCall(glBindTexture(GL_TEXTURE_BUFFER, mOccupancyTex));
    GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_R8UI, mOccupancyBuffer));

    GLCall(glGenTextures(1, &mTextureArray));
    glBindTexture(GL_TEXTURE_3D, mTextureArray);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, Brickmap::BRICK_SIZE * GRID_SIZE,
                 Brickmap::BRICK_SIZE * GRID_SIZE, Brickmap::BRICK_SIZE * GRID_SIZE, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SPARSE_ARB, GL_TRUE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

BrickgridGpu::~BrickgridGpu()
{
    glDeleteBuffers(1, &mOccupancyBuffer);
    glDeleteTextures(1, &mOccupancyTex);
}

int BrickgridGpu::lastNumberOfRayIterations()
{
    return mLastNumberOfRayIterations;
}

void BrickgridGpu::updateCounters()
{

    mLastNumberOfRayIterations = mAtomicCounter.read();
    mAtomicCounter.reset();
}

size_t BrickgridGpu::index(size_t x, size_t y, size_t z)
{
    return x * GRID_SIZE * GRID_SIZE + y * GRID_SIZE + z;
}

std::bitset<Voxino::BrickgridGpu::GRID_VOLUME> BrickgridGpu::occupancyBitset()
{
    return mOccupancyBitset;
}

const std::unique_ptr<Brickmap>& BrickgridGpu::getBrickmap(size_t x, size_t y, size_t z)
{
    return mGrid[index(x, y, z)];
}

void BrickgridGpu::setBrickmap(size_t x, size_t y, size_t z, std::unique_ptr<Brickmap>&& brickmap)
{
    auto gridIndex = index(x, y, z);
    mGrid[gridIndex] = std::move(brickmap);
    if (mGrid[gridIndex]) [[likely]]
    {
        mOccupancyBitset.set(gridIndex, true);
    }
    else
    {
        mOccupancyBitset.set(gridIndex, false);
    }
    mNeedsBufferUpdate = true;
}

void BrickgridGpu::removeBrickmap(size_t x, size_t y, size_t z)
{
    auto gridIndex = index(x, y, z);
    mGrid[gridIndex].reset();
    mOccupancyBitset.set(gridIndex, false);
    mNeedsBufferUpdate = true;
}

void BrickgridGpu::update()
{
    if (mNeedsBufferUpdate)
    {
        std::vector<unsigned char> buffer(GRID_VOLUME, 0);
        for (size_t i = 0; i < GRID_VOLUME; ++i)
        {
            buffer[i] = mOccupancyBitset.test(i);
        }
        GLCall(glBindBuffer(GL_TEXTURE_BUFFER, mOccupancyBuffer));
        GLCall(glBufferData(GL_TEXTURE_BUFFER, buffer.size() * sizeof(unsigned char), buffer.data(),
                            GL_STATIC_DRAW));

        // If updating part of the buffer, for example, the ith element:
        // glBufferSubData(GL_TEXTURE_BUFFER, i * sizeof(int), sizeof(int), &occupancy[i]);

        GLCall(glBindTexture(GL_TEXTURE_3D, mTextureArray));
        for (int i = 0; i < GRID_VOLUME; ++i)
        {
            if (mGrid[i])
            {
                // Calculate the 3D grid position based on the linear index
                int z = i / (GRID_SIZE * GRID_SIZE);
                int y = (i / GRID_SIZE) % GRID_SIZE;
                int x = i % GRID_SIZE;

                // Calculate the offsets for the texture
                int xOffset = x * Brickmap::BRICK_SIZE;
                int yOffset = y * Brickmap::BRICK_SIZE;
                int zOffset = z * Brickmap::BRICK_SIZE;

                // Check if memory is committed for this brick, if using sparse textures
                glTexPageCommitmentARB(GL_TEXTURE_3D, 0, xOffset, yOffset, zOffset,
                                       Brickmap::BRICK_SIZE, Brickmap::BRICK_SIZE,
                                       Brickmap::BRICK_SIZE, GL_TRUE);

                // Now upload the texture data for this specific brick
                GLCall(glTexSubImage3D(GL_TEXTURE_3D, 0, xOffset, yOffset, zOffset,
                                       Brickmap::BRICK_SIZE, Brickmap::BRICK_SIZE,
                                       Brickmap::BRICK_SIZE, GL_RGBA, GL_UNSIGNED_BYTE,
                                       mGrid[i]->textureIds.data()));
            }
        }
    }
}

void BrickgridGpu::draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    if (not mIsDataFilled and not mWasDataFiledLogPrinted) [[unlikely]]
    {
        spdlog::warn("Trying to display Voxel Data without filling it first.");
        mWasDataFiledLogPrinted = true;
    }

    shader.bind();
    shader.setUniform("u_BrickSize", Brickmap::BRICK_SIZE);
    shader.setUniform("u_GridSize", GRID_SIZE);
    mAtomicCounter.bind();
    GLCall(glActiveTexture(GL_TEXTURE1));
    glBindTexture(GL_TEXTURE_3D, mTextureArray);
    shader.setUniform("u_Brickmaps", 1);

    GLCall(glActiveTexture(GL_TEXTURE2));
    GLCall(glBindTexture(GL_TEXTURE_BUFFER, mOccupancyTex));
    shader.setUniform("u_BrickmapOccupancy", 2);
    shader.setUniform("u_WorldSize", glm::ivec3(ChunkBlocks::BLOCKS_PER_DIMENSION,
                                                ChunkBlocks::BLOCKS_PER_DIMENSION,
                                                ChunkBlocks::BLOCKS_PER_DIMENSION));
    renderer.drawRaycast(shader, camera);
}

}// namespace Voxino