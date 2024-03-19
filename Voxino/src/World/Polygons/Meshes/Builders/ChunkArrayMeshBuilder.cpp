#include "ChunkArrayMeshBuilder.h"
namespace Voxino
{
ChunkArrayMeshBuilder::ChunkArrayMeshBuilder()
    : ChunkMeshBuilder()
    , mMesh(std::make_unique<ChunkArrayMesh>())
{
}

ChunkArrayMeshBuilder::ChunkArrayMeshBuilder(Block::Coordinate origin)
    : ChunkMeshBuilder(origin)
    , mMesh(std::make_unique<ChunkArrayMesh>())
{
}

void ChunkArrayMeshBuilder::addQuad(const Block::Face& blockFace, Block::TextureId blockId,
                                    const Block::Coordinate& blockPosition)
{
    auto textureQuad = std::vector{
        glm::vec2(1, 1),//
        glm::vec2(0, 1),//
        glm::vec2(0, 0),//
        glm::vec2(1, 0) //
    };
    auto& indices = mMesh->indices;
    for (auto i = 0; i < 4; ++i)
    {
        auto& vertex = mMesh->vertices.emplace_back();
        vertex.position = addBlockFaceVertices(blockFace, blockPosition, i);
        vertex.textureCoordinates = textureQuad[i];
        vertex.textureId = static_cast<float>(blockId);
        vertex.directionalLightning = addBlockFaceFakeLightning(blockFace);
    }
    addBlockFaceIndices(indices);
}

void ChunkArrayMeshBuilder::addQuad(const MeshRegion& move)
{
    auto& indices = mMesh->indices;
    for (auto i = 0; i < 4; ++i)
    {
        auto& vertex = mMesh->vertices.emplace_back();
        vertex.position =
            addBlockFaceVertices(move.face, move.blockPosition, i, move.width, move.height);
        vertex.textureCoordinates = move.textureCoordinates[i];
        vertex.directionalLightning = addBlockFaceFakeLightning(move.face);
        vertex.textureId = static_cast<float>(move.id);
    }
    addBlockFaceIndices(indices);
}

glm::vec3 ChunkArrayMeshBuilder::addBlockFaceVertices(const Block::Face& blockFace,
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

glm::vec3 ChunkArrayMeshBuilder::addBlockFaceVertices(const Block::Face& blockFace,
                                                      const Block::Coordinate& blockPosition, int i,
                                                      float width, float height) const
{
    /*
     * Some blocks are larger than others.
     * It would be good if they were not just longer in one plane,
     * but it was spread out among all of them -- increased relative to the center.
     */
    const auto blockSizeDifference = 0;

    const auto mBlockFaceSize = Block::BLOCK_SIZE;

    auto face = faceVertices(blockFace);
    const auto& originPos = mOrigin.nonBlockMetric();
    const auto& blockPos = blockPosition.nonBlockMetric();

    float widthFactor, heightFactor, depthFactor;
    switch (blockFace)
    {
        case Block::Face::Top:
        case Block::Face::Bottom:
            // For top and bottom, width spans x and height spans z
            widthFactor = width;
            heightFactor = 1.0;
            depthFactor = height;
            break;
        case Block::Face::Left:
        case Block::Face::Right:
            widthFactor = 1.0;
            heightFactor = height;
            depthFactor = width;
            break;
        case Block::Face::Front:
        case Block::Face::Back:
            // For front and back, width spans x and height spans y
            widthFactor = width;
            heightFactor = height;
            depthFactor = 1.0;
            break;
        default: throw std::runtime_error("Unsupported Block::Face value was provided");
    }

    i *= 3;
    return {(face[i] * widthFactor + originPos.x + blockPos.x),
            (face[i + 1] * heightFactor + originPos.y + blockPos.y),
            (face[i + 2] * depthFactor + originPos.z + blockPos.z)};
}


void ChunkArrayMeshBuilder::resetMesh()
{
    mMesh = std::make_unique<ChunkArrayMesh>();
    mIndex = 0;
}

std::unique_ptr<Mesh3D> ChunkArrayMeshBuilder::mesh3D()
{
    return mMesh->clone();
}
}// namespace Voxino