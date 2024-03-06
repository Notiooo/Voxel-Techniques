#include "MeshBuilder.h"
#include "pch.h"
namespace Voxino
{
MeshBuilder::MeshBuilder(Block::Coordinate origin)
    : mOrigin(std::move(origin))
{
}

MeshBuilder::MeshBuilder()
    : mOrigin(Block::Coordinate{0, 0, 0})
{
}
}// namespace Voxino