#include "ChunkAtlasMesh.h"

namespace Voxino::Polygons
{
VertexBuffer ChunkAtlasMesh::vertexBuffer()
{
    VertexBuffer vb(vertices);
    return vb;
}

void ChunkAtlasMesh::reset()
{
    vertices.clear();
    indices.clear();
}

BufferLayout ChunkAtlasMesh::bufferLayout()
{
    BufferLayout bl;
    bl.push<float>(3);
    bl.push<float>(2);
    // bl.push<float>(1); // directionalLightning
    return bl;
}

std::unique_ptr<Mesh3D> ChunkAtlasMesh::clone()
{
    return std::make_unique<ChunkAtlasMesh>(*this);
}

int ChunkAtlasMesh::numberOfVertices()
{
    return vertices.size();
}
unsigned long ChunkAtlasMesh::memorySize()
{
    return sizeof(VertexData) * vertices.size();
}

}// namespace Voxino::Polygons