#include "ChunkAtlasMeshBuilder.h"

namespace Voxino::Polygons
{
ChunkAtlasMeshBuilder::ChunkAtlasMeshBuilder()
    : ChunkMeshBuilder()
    , mMesh(std::make_unique<ChunkAtlasMesh>())
{
}

ChunkAtlasMeshBuilder::ChunkAtlasMeshBuilder(Block::Coordinate origin)
    : ChunkMeshBuilder(origin)
    , mMesh(std::make_unique<ChunkAtlasMesh>())
{
}

void ChunkAtlasMeshBuilder::addQuad(const Block::Face& blockFace,
                                    const std::vector<glm::vec2>& textureQuad,
                                    const Block::Coordinate& blockPosition)
{
    auto& indices = mMesh->indices;
    for (auto i = 0; i < 4; ++i)
    {
        auto& vertex = mMesh->vertices.emplace_back();
        vertex.position = addBlockFaceVertices(blockFace, blockPosition, i);
        vertex.textureCoordinates = textureQuad[i];
        // vertex.directionalLightning = addBlockFaceFakeLightning(blockFace);
    }
    addBlockFaceIndices(indices);
}

glm::vec3 ChunkAtlasMeshBuilder::addBlockFaceVertices(const Block::Face& blockFace,
                                                      const Block::Coordinate& blockPosition,
                                                      int i) const
{
    /*
     * Some blocks are larger than others.
     * It would be good if they were not just longer in one plane,
     * but it was spread out among all of them -- increased relative to the center.
     */
    const auto blockSizeDifference = 0;// none now
    const auto mBlockFaceSize = Block::BLOCK_SIZE;

    auto face = faceVertices(blockFace);
    const auto& originPos = mOrigin.nonBlockMetric();
    const auto& blockPos = blockPosition.nonBlockMetric();

    i *= 3;
    return {face[i] * mBlockFaceSize + originPos.x + blockPos.x - blockSizeDifference,
            face[i + 1] * mBlockFaceSize + originPos.y + blockPos.y - blockSizeDifference,
            face[i + 2] * mBlockFaceSize + originPos.z + blockPos.z - blockSizeDifference};
}

void ChunkAtlasMeshBuilder::resetMesh()
{
    mMesh = std::make_unique<ChunkAtlasMesh>();
    mIndex = 0;
}

std::unique_ptr<Mesh3D> ChunkAtlasMeshBuilder::mesh3D()
{
    return mMesh->clone();
}
}// namespace Voxino::Polygons