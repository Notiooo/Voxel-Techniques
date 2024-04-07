#pragma once

#include "World/Polygons/Block/Block.h"
#include "World/Polygons/Chunks/ChunkInterface.h"
#include "World/Polygons/Chunks/SimpleTerrainGenerator.h"
#include "World/Polygons/Meshes/Builders/ChunkMeshBuilder.h"
#include "World/Polygons/Meshes/Model3D.h"
#include <Resources/TexturePackArray.h>
#include <World/Polygons/Meshes/Builders/ChunkArrayMeshBuilder.h>
#include <memory>
#include <optional>
#include <vector>

namespace Voxino
{
class ChunkBlocks;
class SimpleTerrainGenerator;
class Renderer;
class Shader;
class ChunkContainer;

/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class VoxelsGpu
{
public:
    VoxelsGpu(int width, int height, int depth);
    ~VoxelsGpu();

    /**
     * \brief Draws the 3D texture to the screen.
     * \param renderer The renderer to draw with.
     * \param shader The shader to draw with.
     * \param camera The camera to draw with.
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const;

private:
    /**
     * \brief Creates a 3D texture.
     */
    void create3DTexture();

    /**
     * \brief Fills the 3D texture with random data.
     */
    void fill3DTexture();

private:
    GLuint mTextureId{};
    int mWidth{};
    int mHeight{};
    int mDepth{};
};
}// namespace Voxino