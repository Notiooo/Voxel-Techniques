#include "VoxelsGpu.h"
#include "pch.h"

namespace Voxino
{
VoxelsGpu::VoxelsGpu(int width, int height, int depth)
    : mWidth(width)
    , mHeight(height)
    , mDepth(depth)
{
    create3DTexture();
    fill3DTexture();
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
}

void VoxelsGpu::fill3DTexture()
{
    MEASURE_SCOPE;

    glBindTexture(GL_TEXTURE_3D, mTextureId);

    std::vector<GLubyte> data(mWidth * mHeight * mDepth * 4);
    static std::random_device rd;
    static std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0, 255);
    std::uniform_int_distribution<> alphaDistr(0, 99);

    for (size_t i = 0; i < data.size(); i += 4)
    {
        data[i] = static_cast<GLubyte>(distr(eng));
        data[i + 1] = static_cast<GLubyte>(distr(eng));
        data[i + 2] = static_cast<GLubyte>(distr(eng));
        data[i + 3] = alphaDistr(eng) < 5 ? 255 : 0;// 5% chance for 255
    }

    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, mWidth, mHeight, mDepth, GL_RGBA, GL_UNSIGNED_BYTE,
                    data.data());
}

void VoxelsGpu::draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;

    shader.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, mTextureId);
    shader.setUniform("u_VoxelWorld", 0);
    shader.setUniform("u_WorldSize", glm::ivec3(mWidth, mHeight, mDepth));
    renderer.drawRaycast(shader, camera);
}
}// namespace Voxino
