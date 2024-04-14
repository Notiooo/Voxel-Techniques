#pragma once

#include "World/Block/Block.h"
#include "World/Polygons/Meshes/Builders/MeshBuilder.h"
#include "World/Polygons/Meshes/ChunkAtlasMesh.h"
#include "pch.h"

namespace Voxino::Polygons
{
/**
 * It can generate a mesh of typical block inside the game, which can then be projected onto the
 * screen as a 3D object.
 */
class ChunkMeshBuilder : public MeshBuilder
{
    using MeshBuilder::MeshBuilder;

protected:
    /**
     * Returns the vertices for a given block face
     * @param blockFace The face of the block
     * @return The vertices for a given block face
     */
    [[nodiscard]] std::vector<GLfloat> faceVertices(const Block::Face& blockFace) const;

    /**
     * @brief Adds indices of typical block face
     * @param indices Indices to which new indices are to be added
     */
    void addBlockFaceIndices(std::vector<GLuint>& indices);

    /**
     * @brief Adds false lighting to block wall
     * @param blockFace Block face ID
     */
    [[nodiscard]] float addBlockFaceFakeLightning(const Block::Face& blockFace) const;
};
}// namespace Voxino::Polygons