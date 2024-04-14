#include "ChunkBlocks.h"
#include "pch.h"


namespace Voxino
{

ChunkBlocksIterator ChunkBlocks::begin()
{
    return {mBlocks, 0};
}

ChunkBlocksIterator ChunkBlocks::end()
{
    return {mBlocks, BLOCKS_IN_CHUNK};
}

ConstChunkBlocksIterator ChunkBlocks::cbegin() const
{
    return {mBlocks, 0};
}

ConstChunkBlocksIterator ChunkBlocks::cend() const
{
    return {mBlocks, BLOCKS_IN_CHUNK};
}


}// namespace Voxino