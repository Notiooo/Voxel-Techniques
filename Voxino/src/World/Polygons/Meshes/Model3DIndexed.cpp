#include "Model3DIndexed.h"
#include "pch.h"

namespace Voxino::Polygons
{

void Model3DIndexed::setMesh(std::unique_ptr<Mesh3D> mesh)
{
    mMesh = std::move(mesh);
    mVertexBuffer = mMesh->vertexBuffer();
    mBufferLayout = mMesh->bufferLayout();
    mVertexArray.setBuffer(mVertexBuffer, mBufferLayout);
    mIndices.setBuffer(mMesh->indices.data(), static_cast<unsigned int>(mMesh->indices.size()));

#ifdef _DEBUG
    mVertexArray.unbind();
    mIndices.unbind();
#endif
}

void Model3DIndexed::draw(const Renderer& renderer, const Shader& shader,
                          const Camera& camera) const
{
    renderer.draw3D(mVertexArray, mIndices, shader, camera);
}

void Model3DIndexed::draw(const Renderer& renderer, const Shader& shader, const Camera& camera,
                          const Renderer::DrawMode& drawMode) const
{
    renderer.draw3D(mVertexArray, mIndices, shader, camera, drawMode);
}

}// namespace Voxino::Polygons