#pragma once

#include "World/Chunks/Chunk.h"

namespace Voxino
{
class ChunkBlocks;
class SimpleTerrainGenerator;
class Renderer;
class Shader;
class ChunkContainer;

namespace Polygons
{
class Model3D;

/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class PolygonChunk : public Chunk
{
public:
    using Chunk::Chunk;

    /**
     * Draws this chunk to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     * @param camera Camera through which the game world is viewed
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const override;

    /**
     * \brief Prepares/generates the mesh chunk, but does not replace it yet.
     */
    virtual void prepareMesh() = 0;

    /**
     * \brief Swaps the current chunk mesh with the latest, most recently generated one
     */
    virtual void updateMesh() = 0;

    /**
     * It is rebuilding this mesh fresh. Very expensive operation
     */
    virtual void rebuildMesh() = 0;

    /**
     * Draws this chunk terrain to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     * @param camera Camera through which the game world is viewed
     */
    virtual void drawTerrain(const Renderer& renderer, const Shader& shader,
                             const Camera& camera) const;

    /**
     * Draws this chunk liquids to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     * @param camera Camera through which the game world is viewed
     */
    virtual void drawLiquids(const Renderer& renderer, const Shader& shader,
                             const Camera& camera) const;

    /**
     * Draws this chunk floral to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     * @param camera Camera through which the game world is viewed
     */
    virtual void drawFlorals(const Renderer& renderer, const Shader& shader,
                             const Camera& camera) const;

protected:
    std::unique_ptr<Model3D> mTerrainModel;
    std::unique_ptr<Model3D> mFluidModel;
    std::unique_ptr<Model3D> mFloralModel;
};

}// namespace Polygons
}// namespace Voxino