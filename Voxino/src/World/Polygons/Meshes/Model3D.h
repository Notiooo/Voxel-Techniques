#pragma once
#include "Renderer/Core/Buffers/BufferLayout.h"
#include "Renderer/Core/Buffers/IndexBuffer.h"
#include "Renderer/Core/Buffers/VertexBuffer.h"
#include "Renderer/Core/VertexArray.h"
#include "Renderer/Renderer.h"
#include "World/Polygons/Meshes/Mesh3D.h"

namespace Voxino
{
/**
 * 3D model consisting of a mesh, which allows to directly draw on the screen.
 */
class Model3D
{
public:
    Model3D() = default;
    Model3D(Model3D&&) noexcept = default;

    /**
     * \brief This function expects that mesh data
     * will be available through all Model3D existance
     * \param mesh Mesh to display
     */
    void setMesh(std::unique_ptr<Mesh3D> mesh);

    /**
     * Draws this 3D Model to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const;

    /**
     * Draws this 3D Model to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera,
              const Renderer::DrawMode& drawMode) const;

    /**
     * @brief Sets the given buffer layout for this model which represents the arrangement of data
     * in memory.
     * @param bufferLayout BufferLayout which represents the arrangement of data.
     */
    void setLayout(const BufferLayout& bufferLayout);

private:
    VertexArray mVertexArray;
    BufferLayout mBufferLayout;
    IndexBuffer mIndices;
    VertexBuffer mVertexBuffer;
    std::unique_ptr<Mesh3D> mMesh;
};
}// namespace Voxino