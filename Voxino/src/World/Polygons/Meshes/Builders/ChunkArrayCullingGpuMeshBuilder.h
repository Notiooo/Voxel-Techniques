#pragma once

#include "World/Block/Block.h"
#include "World/Polygons/Meshes/Builders/ChunkMeshBuilder.h"
#include "World/Polygons/Meshes/ChunkArrayCullingGpuMesh.h"

#include <World/Polygons/Meshes/Model3DNoIndexes.h>


namespace Voxino::Polygons
{

/**
 * It can generate a mesh of typical block inside the game, which can then be projected onto the
 * screen as a 3D object.
 */
class ChunkArrayCullingGpuMeshBuilder : public ChunkMeshBuilder
{
public:
    using ModelType = Model3DNoIndexes;

    explicit ChunkArrayCullingGpuMeshBuilder(Block::Coordinate origin);
    ChunkArrayCullingGpuMeshBuilder();

    /**
     * Resets the builder state, clearing any progress and setting it back to initial conditions.
     */
    void resetMesh() override;

    /**
     * Generates and retrieves the 3D mesh based on the added blocks and modifications.
     * @return A unique pointer to the created 3D mesh, ready for rendering.
     */
    [[nodiscard]] std::unique_ptr<Mesh3D> mesh3D() override;

    /**
     * Adds a quad to the mesh in place of the designated face at the given coordinates and with the
     * given quad texture.
     * @param blockFace A block face to add
     * @param blockTextureId Texture identifier of block
     * @param blockPosition The position on which the quad will be added
     */
    void addPoint(const Block::Face& blockFace, Block::TextureId blockTextureId,
                  const Block::Coordinate& blockPosition);

protected:
    /* ==== Members ===== */
    std::unique_ptr<ChunkArrayCullingGpuMesh> mMesh;
};
}// namespace Voxino::Polygons