#include "WorldBlockMesh.h"
namespace Voxino
{
VertexBuffer WorldBlockMesh::vertexBuffer()
{
    VertexBuffer vb(vertices);
    return vb;
}

void WorldBlockMesh::reset()
{
    vertices.clear();
    indices.clear();
}

BufferLayout WorldBlockMesh::bufferLayout()
{
    BufferLayout bl;
    bl.push<float>(3);
    bl.push<float>(2);
    bl.push<float>(1);
    return bl;
}

std::unique_ptr<Mesh3D> WorldBlockMesh::clone()
{
    return std::make_unique<WorldBlockMesh>(*this);
}
}// namespace Voxino