#pragma once

#include "World/Polygons/Meshes/Mesh3D.h"
#include <memory>

namespace Voxino
{
/**
 * @brief This is the mesh of a typical block located inside the game. Such a block consists of
 * vertices, textures and shadows on each block face.
 */
struct ChunkAtlasMesh : public Mesh3D
{
    /**
     * @brief Returns the Vertex Buffer that creates the mesh
     * @return Vertex Buffer that creates the mesh
     */
    VertexBuffer vertexBuffer() override;

    /**
     * @brief Resets mesh to initial values (clears it).
     */
    void reset() override;

    /**
     * @brief Returns BufferLayout, which determines the memory layout of a given mesh
     * @return BufferLayout, which determines the memory layout of a given mesh
     */
    BufferLayout bufferLayout() override;

    /**
     * @brief Copies the mesh and returns a pointer to its copy
     * @return Pointer to mesh copy
     */
    std::unique_ptr<Mesh3D> clone() override;

    /**
     * Returns the number of vertices.
     * @return Number of vertices.
     */
    int numberOfVertices() override;

    /**
     * Returns the size in memory that the mesh occupies
     * @return The size in bytes in memory that the mesh occupies
     */
    int memorySize() override;

    /* ==== Members ===== */
    struct VertexData
    {
        glm::vec3 position{};
        glm::vec2 textureCoordinates{};
        float directionalLightning{};
    };
    std::vector<VertexData> vertices;
};
}// namespace Voxino