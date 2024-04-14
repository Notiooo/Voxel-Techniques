#include "ChunkArrayMesh.h"

namespace Voxino::Polygons
{
VertexBuffer ChunkArrayMesh::vertexBuffer()
{
    VertexBuffer vb(vertices);
    return vb;
}

void ChunkArrayMesh::reset()
{
    vertices.clear();
    indices.clear();
}

BufferLayout ChunkArrayMesh::bufferLayout()
{
    BufferLayout bl;
    bl.push<float>(3);
    bl.push<float>(2);
    bl.push<float>(1);
    bl.push<float>(1);
    return bl;
}

std::unique_ptr<Mesh3D> ChunkArrayMesh::clone()
{
    return std::make_unique<ChunkArrayMesh>(*this);
}

int ChunkArrayMesh::numberOfVertices()
{
    return vertices.size();
}

int ChunkArrayMesh::memorySize()
{
    return sizeof(VertexData) * vertices.size();
}

}// namespace Voxino::Polygons