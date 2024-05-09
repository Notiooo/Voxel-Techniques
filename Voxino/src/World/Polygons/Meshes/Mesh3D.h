#pragma once
#include "Renderer/Core/Buffers/BufferLayout.h"
#include "Renderer/Core/Buffers/VertexBuffer.h"
#include <vector>

namespace Voxino::Polygons
{
/**
 * Mesh3D is the abstract concept of a 3D mesh that can be projected onto the screen by a model.
 */
struct Mesh3D
{
    virtual ~Mesh3D() = default;

    /**
     * @brief Returns the Vertex Buffer that creates the mesh
     * @return Vertex Buffer that creates the mesh
     */
    virtual VertexBuffer vertexBuffer() = 0;

    /**
     * @brief Returns BufferLayout, which determines the memory layout of a given mesh
     * @return BufferLayout, which determines the memory layout of a given mesh
     */
    virtual BufferLayout bufferLayout() = 0;

    /**
     * @brief Resets mesh to initial values (clears it).
     */
    virtual void reset() = 0;

    /**
     * @brief Copies the mesh and returns a pointer to its copy
     * @return Pointer to mesh copy
     */
    virtual std::unique_ptr<Mesh3D> clone() = 0;

    /**
     * Returns the number of vertices.
     * @return Number of vertices.
     */
    virtual int numberOfVertices() = 0;

    /**
     * The size in memory that the mesh occupies
     */
    virtual unsigned long memorySize() = 0;

    /* ==== Members ===== */
    std::vector<GLuint> indices;
};
}// namespace Voxino::Polygons