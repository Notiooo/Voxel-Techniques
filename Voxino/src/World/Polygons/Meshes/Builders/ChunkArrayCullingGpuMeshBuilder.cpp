#include "ChunkArrayCullingGpuMeshBuilder.h"
namespace Voxino::Polygons
{
ChunkArrayCullingGpuMeshBuilder::ChunkArrayCullingGpuMeshBuilder()
    : ChunkMeshBuilder()
    , mMesh(std::make_unique<ChunkArrayCullingGpuMesh>())
{
}

ChunkArrayCullingGpuMeshBuilder::ChunkArrayCullingGpuMeshBuilder(Block::Coordinate origin)
    : ChunkMeshBuilder(origin)
    , mMesh(std::make_unique<ChunkArrayCullingGpuMesh>())
{
}

void ChunkArrayCullingGpuMeshBuilder::addPoint(const Block::Face& blockFace,
                                               Block::TextureId blockTextureId,
                                               const Block::Coordinate& blockPosition)
{
    const auto& originPos = mOrigin.nonBlockMetric();
    const auto& blockPos = blockPosition.nonBlockMetric();
    const auto& position = originPos + blockPos;

    auto& vertex = mMesh->vertices.emplace_back();
    vertex.position = {position.x, position.y, position.z};
    vertex.textureId = static_cast<int>(blockTextureId);
    vertex.face = static_cast<int>(blockFace);
    // vertex.directionalLightning = addBlockFaceFakeLightning(blockFace);
}

void ChunkArrayCullingGpuMeshBuilder::resetMesh()
{
    mMesh = std::make_unique<ChunkArrayCullingGpuMesh>();
    mIndex = 0;
}

std::unique_ptr<Mesh3D> ChunkArrayCullingGpuMeshBuilder::mesh3D()
{
    return mMesh->clone();
}
}// namespace Voxino::Polygons