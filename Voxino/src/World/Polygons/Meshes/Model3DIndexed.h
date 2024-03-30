#pragma once
#include "Renderer/Core/Buffers/BufferLayout.h"
#include "Renderer/Core/Buffers/IndexBuffer.h"
#include "Renderer/Core/Buffers/VertexBuffer.h"
#include "Renderer/Core/VertexArray.h"
#include "Renderer/Renderer.h"
#include "World/Polygons/Meshes/Mesh3D.h"
#include "World/Polygons/Meshes/Model3D.h"

namespace Voxino
{
/**
 * 3D model consisting of a mesh, which allows to directly draw on the screen.
 */
class Model3DIndexed : public Model3D
{
public:
    /**
     * \brief This function expects that mesh data
     * will be available through all Model3DNoIndexes existance
     * \param mesh Mesh to display
     */
    void setMesh(std::unique_ptr<Mesh3D> mesh) override;

    /**
     * Draws this 3D Model to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const override;

    /**
     * Draws this 3D Model to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera,
              const Renderer::DrawMode& drawMode) const override;

private:
    VertexArray mVertexArray;
    IndexBuffer mIndices;
    VertexBuffer mVertexBuffer;
};
}// namespace Voxino