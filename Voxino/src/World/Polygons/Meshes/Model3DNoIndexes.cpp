#include "Model3DNoIndexes.h"
#include "pch.h"

namespace Voxino::Polygons
{

void Model3DNoIndexes::setMesh(std::unique_ptr<Mesh3D> mesh)
{
    mMesh = std::move(mesh);
    mVertexBuffer = mMesh->vertexBuffer();
    mBufferLayout = mMesh->bufferLayout();
    mVertexArray.setBuffer(mVertexBuffer, mBufferLayout);

#ifdef _DEBUG
    mVertexArray.unbind();
#endif
}

void Model3DNoIndexes::draw(const Renderer& renderer, const Shader& shader,
                            const Camera& camera) const
{
    renderer.draw3D(mVertexArray, mMesh->numberOfVertices(), shader, camera);
}

void Model3DNoIndexes::draw(const Renderer& renderer, const Shader& shader, const Camera& camera,
                            const Renderer::DrawMode& drawMode) const
{
    renderer.draw3D(mVertexArray, mMesh->numberOfVertices(), shader, camera, drawMode);
}

}// namespace Voxino::Polygons