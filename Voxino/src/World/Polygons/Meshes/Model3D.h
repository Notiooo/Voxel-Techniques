#pragma once
#include "Renderer/Core/Buffers/BufferLayout.h"
#include "Renderer/Core/Buffers/IndexBuffer.h"
#include "Renderer/Core/Buffers/VertexBuffer.h"
#include "Renderer/Core/VertexArray.h"
#include "Renderer/Renderer.h"
#include "World/Polygons/Meshes/Mesh3D.h"

namespace Voxino::Polygons
{
/**
 * 3D model consisting of a mesh, which allows to directly draw on the screen.
 */
class Model3D
{
public:
    Model3D() = default;
    Model3D(Model3D&&) noexcept = default;
    virtual ~Model3D() = default;

    /**
     * \brief This function expects that mesh data
     * will be available through all Model3D existance
     * \param mesh Mesh to display
     */
    virtual void setMesh(std::unique_ptr<Mesh3D> mesh) = 0;

    /**
     * Draws this 3D Model to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    virtual void draw(const Renderer& renderer, const Shader& shader,
                      const Camera& camera) const = 0;

    /**
     * Draws this 3D Model to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    virtual void draw(const Renderer& renderer, const Shader& shader, const Camera& camera,
                      const Renderer::DrawMode& drawMode) const = 0;

    /**
     * @brief Sets the given buffer layout for this model which represents the arrangement of data
     * in memory.
     * @param bufferLayout BufferLayout which represents the arrangement of data.
     */
    void setLayout(const BufferLayout& bufferLayout);

    /**
     * Provides non-const access to the Mesh3D instance associated with this object.
     * @return Reference to the Mesh3D instance.
     */
    [[nodiscard]] Mesh3D& mesh();

    /**
     * Provides const access to the Mesh3D instance associated with this object.
     * @return Const reference to the Mesh3D instance.
     */
    [[nodiscard]] const Mesh3D& mesh() const;

protected:
    BufferLayout mBufferLayout;
    std::unique_ptr<Mesh3D> mMesh;
};
}// namespace Voxino::Polygons