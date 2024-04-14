#pragma once
#include "Block.h"

namespace Voxino
{

/**
 * It carries with it information about the type of block.
 */
struct BlockType
{
    BlockId id;
    std::string name;
    bool transparent = false;
    bool collidable = true;
    std::unordered_map<Block::Face, Block::TextureId> textureId;
};

}// namespace Voxino