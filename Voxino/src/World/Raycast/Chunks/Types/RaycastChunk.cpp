#include "RaycastChunk.h"
#include "pch.h"

#include <Utils/RGBA.h>

namespace Voxino::Raycast
{

RaycastChunk::RaycastChunk(const Block::Coordinate& blockPosition,
                           const TexturePackArray& texturePack)
    : Chunk(blockPosition, texturePack)
    , mVoxels(ChunkBlocks::BLOCKS_PER_X_DIMENSION, ChunkBlocks::BLOCKS_PER_Y_DIMENSION,
              ChunkBlocks::BLOCKS_PER_Z_DIMENSION)
{
    fillData();
}

int RaycastChunk::numberOfVertices()
{
    return 0;
}

int RaycastChunk::memorySize()
{
    return 0;
}

void RaycastChunk::fillData()
{
    MEASURE_SCOPE;
    std::vector<RGBA> data;
    for (const auto& [position, block]: *mChunkOfBlocks)
    {
        if (block.id() == BlockId::Air)
        {
            data.push_back(RGBA{0, 0, 0, 0});
        }
        else
        {
            data.push_back(toRGBA(block));
        }
    }
    mVoxels.fill(data);
}

void RaycastChunk::removeLocalBlock(const Block::Coordinate& localCoordinates)
{
    Chunk::removeLocalBlock(localCoordinates);
    // mVoxels.updateSingleBlock()
}

void RaycastChunk::draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const
{
    mVoxels.draw(renderer, shader, camera);
}

bool RaycastChunk::tryToPlaceBlockInsideThisChunk(const BlockId& blockId,
                                                  const Block::Coordinate& localCoordinates,
                                                  std::vector<BlockId>& blocksThatMightBeOverplaced)
{
    if (Chunk::tryToPlaceBlockInsideThisChunk(blockId, localCoordinates,
                                              blocksThatMightBeOverplaced))
    {
        auto& block = mChunkOfBlocks->block(localCoordinates);
        mVoxels.updateSingleBlock({localCoordinates.x, localCoordinates.y, localCoordinates.z},
                                  toBlockInShader(block));
        return true;
    }
    return false;
}

std::array<GLubyte, 4> RaycastChunk::toBlockInShader(const Block& block)
{
    std::array<GLubyte, 4> rgba{};
    constexpr auto numberOfBlockFaces = 6;
    for (auto i = 0; i < numberOfBlockFaces; i += 2)
    {
        auto textureID1 = block.blockTextureId(static_cast<Block::Face>(i));
        auto textureID2 = block.blockTextureId(static_cast<Block::Face>(i + 1));
        rgba[i / 2] = encodeTextureID(textureID1, textureID2);
    }
    rgba[3] = 255;
    return rgba;
}

float RaycastChunk::encodeTextureID(int highPart, int lowPart)
{
    int value = (highPart << 4) | lowPart;
    return value;
}

RGBA RaycastChunk::toRGBA(const Voxino::Block& block)
{
    auto blockInShader = toBlockInShader(block);
    const auto rgba = reinterpret_cast<RGBA&>(blockInShader);
    return rgba;
}

}// namespace Voxino::Raycast