#pragma once

#include "World/Polygons/Block/Block.h"
#include "World/Polygons/Meshes/Builders/ChunkMeshBuilder.h"
#include "World/Polygons/Meshes/ChunkArrayMesh.h"
#include "pch.h"

#include <World/Polygons/Meshes/MeshRegion.h>


namespace Voxino
{
/**
 * It can generate a mesh of typical block inside the game, which can then be projected onto the
 * screen as a 3D object.
 */
class ChunkArrayMeshBuilder : public ChunkMeshBuilder
{
public:
    explicit ChunkArrayMeshBuilder(Block::Coordinate origin);
    ChunkArrayMeshBuilder();

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
     * Adds a quad to the mesh based on the specified mesh region details.
     * @param move The mesh region information used to define the quad's properties and placement.
     */
    void addQuad(const MeshRegion& move);

    /**
     * Adds a quad to the mesh in place of the designated face at the given coordinates and with the
     * given quad texture.
     * @param blockFace A block face to add
     * @param blockId TODO: THIS
     * @param blockPosition The position on which the quad will be added
     */
    void addQuad(const Block::Face& blockFace, Block::TextureId blockId,
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
    /**
     * Calculates the vertices for a block face within the mesh.
     *
     * This method computes the positions of vertices for a given face of a block,
     * considering the block's position, the dimensions of the face, and a specific index or
     * identifier that may influence the calculation. It's used to generate the vertex data
     * necessary for rendering the block face.
     *
     * @param blockFace The specific face of the block for which vertices are calculated.
     * @param blockPosition The position of the block within the chunk.
     * @param i An index or identifier related to the face's vertex calculation.
     * @param width The width of the block face for which vertices are calculated.
     * @param height The height of the block face for which vertices are calculated.
     * @return The calculated vertices for the specified block face.
     */
    [[nodiscard]] glm::vec3 addBlockFaceVertices(const Block::Face& blockFace,
                                                 const Block::Coordinate& blockPosition, int i,
                                                 float width, float height) const;

protected:
    /* ==== Members ===== */
    std::unique_ptr<ChunkArrayMesh> mMesh;
};
}// namespace Voxino