#include "Block.h"
#include "pch.h"

#include "BlockMap.h"
#include "Renderer/Core/Buffers/BufferLayout.h"


namespace Voxino
{

Block::Block()
    : mBlockType(&BlockMap::blockMap().blockType(BlockId::Air))
{
}

Block::Block(const BlockId& blockId)
    : mBlockType(&BlockMap::blockMap().blockType(blockId))
{
}

Block::Block(const Block& rhs)
    : mBlockType(rhs.mBlockType)
{
}

Block::Coordinate Block::Coordinate::coordinateInGivenDirection(Direction direction) const
{
    switch (direction)
    {
        case Direction::Above: return Block::Coordinate{x, y + 1, z};
        case Direction::Below: return Block::Coordinate{x, y - 1, z};
        case Direction::ToTheLeft: return Block::Coordinate{x - 1, y, z};
        case Direction::ToTheRight: return Block::Coordinate{x + 1, y, z};
        case Direction::InFront: return Block::Coordinate{x, y, z + 1};
        case Direction::Behind: return Block::Coordinate{x, y, z - 1};
        default: throw std::runtime_error("Unsupported direction type");
    }
}

sf::Vector3<Block::SizeType> Block::Coordinate::nonBlockMetric() const
{
    return {static_cast<SizeType>(x) * BLOCK_SIZE, static_cast<SizeType>(y) * BLOCK_SIZE,
            static_cast<SizeType>(z) * BLOCK_SIZE};
}

void Block::setBlockType(const BlockId& blockId)
{
    mBlockType = &BlockMap::blockMap().blockType(blockId);
}

Block::TextureId Block::blockTextureId(const Block::Face& blockFace) const
{
    return mBlockType->textureId.at(blockFace);
}

BlockId Block::id() const
{
    return mBlockType->id;
}

bool Block::isTransparent() const
{
    return mBlockType->transparent;
}

bool Block::isFloral() const
{
    switch (id())
    {
        case BlockId::Leaves:
        case BlockId::SnowyLeaves: return true;
        default: return false;
    }
}

RGBA Block::toRGBA() const
{
    auto blockInShader = toBlockInShader();
    const auto rgba = reinterpret_cast<RGBA&>(blockInShader);
    return rgba;
}

std::array<GLubyte, 4> Block::toBlockInShader() const
{
    std::array<GLubyte, 4> rgba{};
    constexpr auto numberOfBlockFaces = 6;
    for (auto i = 0; i < numberOfBlockFaces; i += 2)
    {
        auto textureID1 = blockTextureId(static_cast<Block::Face>(i));
        auto textureID2 = blockTextureId(static_cast<Block::Face>(i + 1));
        rgba[i / 2] = encodeTextureID(textureID1, textureID2);
    }
    rgba[3] = 255;
    return rgba;
}

float Block::encodeTextureID(int highPart, int lowPart)
{
    int value = (highPart << 4) | lowPart;
    return value;
}

bool Block::isCollidable() const
{
    return mBlockType->collidable;
}

Direction Block::directionOfFace(Block::Face face)
{
    switch (face)
    {
        case Face::Top: return Direction::Above;
        case Face::Left: return Direction::ToTheLeft;
        case Face::Right: return Direction::ToTheRight;
        case Face::Bottom: return Direction::Below;
        case Face::Front: return Direction::InFront;
        case Face::Back: return Direction::Behind;
        default: throw std::runtime_error("Unsupported block face type");
    }
}

}// namespace Voxino