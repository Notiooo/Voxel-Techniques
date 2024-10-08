#pragma once
#include "BlockType.h"

namespace Voxino
{

/**
 * A map containing access to information about the blocks in the game.
 */
class BlockMap
{
public:
    /**
     * Returns an instance of the block map
     * @return Instance of the block map
     */
    const static BlockMap& blockMap();

    /**
     * Returns the type of block, which is information about the block
     * @param blockId The id of the block
     * @return Block information set
     */
    [[nodiscard]] const BlockType& blockType(const BlockId& blockId) const;

private:
    BlockMap();

    /**
     * It searches the folder for block settings.
     * @param directoryName Path to folder in search of blocks
     */
    void parseDirectory(const std::string& directoryName);

    /**
     * Maps filename to specific BlockType
     */
    std::unordered_map<BlockId, BlockType> mBlockMap;
};

}// namespace Voxino