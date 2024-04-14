#include "ChunkArrayCullingGpuMesh.h"

namespace Voxino::Polygons
{
VertexBuffer ChunkArrayCullingGpuMesh::vertexBuffer()
{
    VertexBuffer vb(vertices);
    return vb;
}

void ChunkArrayCullingGpuMesh::reset()
{
    vertices.clear();
    indices.clear();
}

BufferLayout ChunkArrayCullingGpuMesh::bufferLayout()
{
    BufferLayout bl;
    bl.push<float>(3);
    bl.push<float>(1);
    bl.push<float>(1);
    bl.push<float>(1);
    return bl;
}

std::unique_ptr<Mesh3D> ChunkArrayCullingGpuMesh::clone()
{
    return std::make_unique<ChunkArrayCullingGpuMesh>(*this);
}

int ChunkArrayCullingGpuMesh::numberOfVertices()
{
    return vertices.size();
}

int ChunkArrayCullingGpuMesh::memorySize()
{
    return sizeof(VertexData) * vertices.size();
}

}// namespace Voxino::Polygons