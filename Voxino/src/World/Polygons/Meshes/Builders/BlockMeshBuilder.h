#pragma once

#include "World/Polygons/Block/Block.h"
#include "World/Polygons/Meshes/Builders/MeshBuilder.h"
#include "World/Polygons/Meshes/WorldBlockMesh.h"
#include "pch.h"


namespace Voxino
{
/**
 * It can generate a mesh of typical block inside the game, which can then be projected onto the
 * screen as a 3D object.
 */
class BlockMeshBuilder : public MeshBuilder
{
public:
    explicit BlockMeshBuilder(Block::Coordinate origin);
    BlockMeshBuilder();

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
     * Sets the size of the face to be created
     * @param faceSize Block face size
     */
    void setFaceSize(const float& faceSize);

    /**
     * Adds a quad to the mesh in place of the designated face at the given coordinates and with the
     * given quad texture.
     * @param blockFace A block face to add
     * @param textureQuad Position in the texture pack of the texture to be displayed
     * @param blockPosition The position on which the quad will be added
     */
    void addQuad(const Block::Face& blockFace, const std::vector<glm::vec2>& textureQuad,
                 const Block::Coordinate& blockPosition);

protected:
    /**
     * Returns the vertices for a given block face
     * @param blockFace The face of the block
     * @return The vertices for a given block face
     */
    [[nodiscard]] std::vector<GLfloat> faceVertices(const Block::Face& blockFace) const;
    float mBlockFaceSize = Block::BLOCK_SIZE;

private:
    /**
     * @brief Adds indices of typical block face
     * @param indices Indices to which new indices are to be added
     */
    void addBlockFaceIndices(std::vector<GLuint>& indices);

    /**
     * @brief For a given face block, it adds vertices building it up
     * @param blockFace Block face ID
     * @param blockPosition Position of the block in space
     */
    [[nodiscard]] glm::vec3 addBlockFaceVertices(const Block::Face& blockFace,
                                                 const Block::Coordinate& blockPosition,
                                                 int i) const;

    /**
     * @brief Adds false lighting to block wall
     * @param blockFace Block face ID
     */
    [[nodiscard]] float addBlockFaceFakeLightning(const Block::Face& blockFace) const;

protected:
    /* ==== Members ===== */
    std::unique_ptr<WorldBlockMesh> mMesh;
};
}// namespace Voxino