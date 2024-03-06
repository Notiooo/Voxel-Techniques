#include "Model3D.h"
#include "pch.h"

namespace Voxino
{

void Model3D::setMesh(std::unique_ptr<Mesh3D> mesh)
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

void Model3D::draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const
{
    renderer.draw3D(mVertexArray, mIndices, shader, camera);
}

void Model3D::draw(const Renderer& renderer, const Shader& shader, const Camera& camera,
                   const Renderer::DrawMode& drawMode) const
{
    renderer.draw3D(mVertexArray, mIndices, shader, camera, drawMode);
}

void Model3D::setLayout(const BufferLayout& bufferLayout)
{
    mBufferLayout = bufferLayout;
}
}// namespace Voxino