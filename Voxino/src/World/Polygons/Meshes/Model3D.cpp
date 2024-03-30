#include "Model3D.h"
#include "pch.h"

namespace Voxino
{

void Model3D::setLayout(const BufferLayout& bufferLayout)
{
    mBufferLayout = bufferLayout;
}

Mesh3D& Model3D::mesh()
{
    return *mMesh;
}

const Mesh3D& Model3D::mesh() const
{
    return *mMesh;
}
}// namespace Voxino