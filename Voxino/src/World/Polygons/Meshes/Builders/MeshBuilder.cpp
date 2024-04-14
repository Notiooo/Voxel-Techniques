#include "MeshBuilder.h"
#include "pch.h"
namespace Voxino::Polygons
{
MeshBuilder::MeshBuilder(Block::Coordinate origin)
    : mOrigin(std::move(origin))
{
}

MeshBuilder::MeshBuilder()
    : mOrigin(Block::Coordinate{0, 0, 0})
{
}
}// namespace Voxino::Polygons