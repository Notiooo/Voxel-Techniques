#pragma once

#include "World/Polygons/Meshes/Builders/ChunkMeshBuilder.h"
#include "World/Polygons/Meshes/Model3D.h"
#include <vector>

namespace Voxino
{
class ChunkBlocks;
class SimpleTerrainGenerator;
class Renderer;
class Shader;
class ChunkContainer;

namespace Raycast
{
/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class VoxelsGpu
{
public:
    template<typename T>
    VoxelsGpu(int width, int height, int depth, const std::vector<T>& data)
        : mWidth(width)
        , mHeight(height)
        , mDepth(depth)
    {
        create3DTexture();
        fill(data);
    }

    VoxelsGpu(int width, int height, int depth);
    ~VoxelsGpu();

    /**
     * \brief Draws the 3D texture to the screen.
     * \param renderer The renderer to draw with.
     * \param shader The shader to draw with.
     * \param camera The camera to draw with.
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const;

    /**
     * \brief Fills the 3D texture with data.
     * @tparam T Type which is made of 4 GLubyte components.
     * @param data The data to fill the 3D texture with.
     */
    template<typename T>
    void fill(const std::vector<T>& data)
    {
        MEASURE_SCOPE;
        mIsDataFilled = true;
        if ((data.size() * sizeof(T)) != mWidth * mHeight * mDepth * 4) [[unlikely]]
        {
            spdlog::critical("3D Texture Data does not match dimensions of the VoxelsGpu object.");
            auto copiedData = data;
            copiedData.resize(mWidth * mHeight * mDepth * 4);
            glBindTexture(GL_TEXTURE_3D, mTextureId);
            glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, mWidth, mHeight, mDepth, GL_RGBA,
                            GL_UNSIGNED_BYTE, copiedData.data());
            return;
        }
        glBindTexture(GL_TEXTURE_3D, mTextureId);
        glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, mWidth, mHeight, mDepth, GL_RGBA,
                        GL_UNSIGNED_BYTE, data.data());
    }

    /**
     * \brief Updates a single block in the 3D texture.
     * \param position The coordinate of the block.
     * \param block The block data.
     */
    void updateSingleBlock(glm::ivec3 position, const GLubyte block[4]);

    /**
     * \brief Updates a single block in the 3D texture.
     * \param position The coordinate of the block.
     * \param block The block data.
     */
    void updateSingleBlock(glm::ivec3 position, const std::array<GLubyte, 4>& block);

    /**
     * \brief Updates a sphere in the 3D texture.
     * \param centerPosition The coordinate of the center of the sphere.
     * \param radius The radius of the sphere.
     * \param block The block data.
     */
    void updateSphere(glm::ivec3 centerPosition, int radius, const GLubyte block[4]);

    /**
     * \brief Resizes the Voxel data.
     * \param width The new width.
     * \param height The new height.
     * \param depth The new depth.
     */
    void resize(int width, int height, int depth);

private:
    /**
     * \brief Creates a 3D texture.
     */
    void create3DTexture();


private:
    GLuint mTextureId{};
    int mWidth{};
    int mHeight{};
    int mDepth{};
    bool mIsDataFilled{false};
    mutable bool mWasDataFiledLogPrinted{false};
};

}// namespace Raycast
}// namespace Voxino