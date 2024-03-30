#pragma once

#include "World/Polygons/Block/Block.h"
#include "World/Polygons/Meshes/Builders/ChunkMeshBuilder.h"
#include "World/Polygons/Meshes/ChunkAtlasMesh.h"
#include "pch.h"

#include <World/Polygons/Meshes/Model3DIndexed.h>

namespace Voxino
{
/**
 * It can generate a mesh of typical block inside the game, which can then be projected onto the
 * screen as a 3D object.
 */
class ChunkAtlasMeshBuilder : public ChunkMeshBuilder
{
public:
    using ModelType = Model3DIndexed;
    explicit ChunkAtlasMeshBuilder(Block::Coordinate origin);
    ChunkAtlasMeshBuilder();

    /**
     * Resets the state of the mesh and returns to the initial values
     */
    void resetMesh() override;

    /**
     * Returns the 3d mesh created
     * @return The created 3D mesh
     */
    [[nodiscard]] std::unique_ptr<Mesh3D> mesh3D() override;

    /**
     * Adds a quad to the mesh in place of the designated face at the given coordinates and with the
     * given quad texture.
     * @param blockFace A block face to add
     * @param textureQuad Position in the texture pack of the texture to be displayed
     * @param blockPosition The position on which the quad will be added
     */
    void addQuad(const Block::Face& blockFace, const std::vector<glm::vec2>& textureQuad,
                 const Block::Coordinate& blockPosition);

private:
    /**
     * @brief For a given face block, it adds vertices building it up
     * @param blockFace Block face ID
     * @param blockPosition Position of the block in space
     */
    [[nodiscard]] glm::vec3 addBlockFaceVertices(const Block::Face& blockFace,
                                                 const Block::Coordinate& blockPosition,
                                                 int i) const;

protected:
    /* ==== Members ===== */
    std::unique_ptr<ChunkAtlasMesh> mMesh;
};
}// namespace Voxino