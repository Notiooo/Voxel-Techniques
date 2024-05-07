#include "VoxelsGpu.h"
#include "pch.h"

namespace Voxino::Raycast
{

VoxelsGpu::VoxelsGpu(int width, int height, int depth)
    : mWidth(width)
    , mHeight(height)
    , mDepth(depth)
{
    create3DTexture();
}

VoxelsGpu::~VoxelsGpu()
{
    glDeleteTextures(0, &mTextureId);
}

void VoxelsGpu::create3DTexture()
{
    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_3D, mTextureId);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, mWidth, mHeight, mDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 NULL);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    mWasDataFiledLogPrinted = false;
    mIsDataFilled = false;
}

void VoxelsGpu::updateSingleBlock(glm::ivec3 position, const GLubyte block[4])
{
    glBindTexture(GL_TEXTURE_3D, mTextureId);
    glTexSubImage3D(GL_TEXTURE_3D, 0, position.x, position.y, position.z, 1, 1, 1, GL_RGBA,
                    GL_UNSIGNED_BYTE, block);
}

void VoxelsGpu::updateSingleBlock(glm::ivec3 position, const std::array<GLubyte, 4>& block)
{
    updateSingleBlock(position, block.data());
}

void VoxelsGpu::updateSphere(glm::ivec3 centerPosition, int radius, const GLubyte block[4])
{
    std::vector<GLubyte> updateData;
    std::vector<std::tuple<int, int, int>> updatePositions;

    // Pre-calculate bounds to ensure they are within texture limits
    int minX = std::max(centerPosition.x - radius, 0);
    int maxX = std::min(centerPosition.x + radius, mWidth - 1);
    int minY = std::max(centerPosition.y - radius, 0);
    int maxY = std::min(centerPosition.y + radius, mHeight - 1);
    int minZ = std::max(centerPosition.z - radius, 0);
    int maxZ = std::min(centerPosition.z + radius, mDepth - 1);

    for (int z = minZ; z <= maxZ; ++z)
    {
        for (int y = minY; y <= maxY; ++y)
        {
            for (int x = minX; x <= maxX; ++x)
            {
                if ((x - centerPosition.x) * (x - centerPosition.x) +
                        (y - centerPosition.y) * (y - centerPosition.y) +
                        (z - centerPosition.z) * (z - centerPosition.z) <=
                    radius * radius)
                {
                    updatePositions.push_back(std::make_tuple(x, y, z));
                }
            }
        }
    }

    if (!updatePositions.empty())
    {
        glBindTexture(GL_TEXTURE_3D, mTextureId);
        for (const auto& pos: updatePositions)
        {
            auto& [x, y, z] = pos;
            glTexSubImage3D(GL_TEXTURE_3D, 0, x, y, z, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, block);
        }
    }
}
void VoxelsGpu::resize(int width, int height, int depth)
{
    glDeleteTextures(1, &mTextureId);
    mWidth = width;
    mHeight = height;
    mDepth = depth;
    create3DTexture();
}

int VoxelsGpu::lastNumberOfRayIterations()
{
    return mLastNumberOfRayIterations;
}

void VoxelsGpu::draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    if (not mIsDataFilled and not mWasDataFiledLogPrinted) [[unlikely]]
    {
        spdlog::warn("Trying to display Voxel Data without filling it first.");
        mWasDataFiledLogPrinted = true;
    }

    shader.bind();
    mAtomicCounter.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, mTextureId);
    shader.setUniform("u_VoxelWorld", 0);
    shader.setUniform("u_WorldSize", glm::ivec3(mWidth, mHeight, mDepth));
    renderer.drawRaycast(shader, camera);
}

void VoxelsGpu::updateCounters()
{
    mLastNumberOfRayIterations = mAtomicCounter.read();
    mAtomicCounter.reset();
}
}// namespace Voxino::Raycast
