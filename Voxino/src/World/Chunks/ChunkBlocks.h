#pragma once

#include "Utils/MultiDimensionalArray.h"
#include "World/Block/Block.h"

namespace Voxino
{

class ChunkBlocksIterator;
class ConstChunkBlocksIterator;

class ChunkBlocks
{
public:
    // For Binary Greedy Meshing to work all of them must be equal
    static constexpr auto BLOCKS_PER_DIMENSION = BLOCK_PER_DIMENSION_IN_CHUNK;
    static constexpr auto BLOCKS_PER_X_DIMENSION = BLOCKS_PER_DIMENSION;
    static constexpr auto BLOCKS_PER_Y_DIMENSION = BLOCKS_PER_DIMENSION;
    static constexpr auto BLOCKS_PER_Z_DIMENSION = BLOCKS_PER_DIMENSION;
    static constexpr auto BLOCKS_IN_CHUNK =
        BLOCKS_PER_X_DIMENSION * BLOCKS_PER_Y_DIMENSION * BLOCKS_PER_Z_DIMENSION;

    [[nodiscard]] ChunkBlocksIterator begin();
    [[nodiscard]] ChunkBlocksIterator end();

    [[nodiscard]] ConstChunkBlocksIterator cbegin() const;
    [[nodiscard]] ConstChunkBlocksIterator cend() const;

    template<typename T>
    inline Block& block(const T& dimensions)
    {
        auto index = (dimensions.z * BLOCKS_PER_Y_DIMENSION * BLOCKS_PER_X_DIMENSION) +
                     (dimensions.y * BLOCKS_PER_X_DIMENSION) + dimensions.x;
        return mBlocks[index];
    }

    template<typename T>
    [[nodiscard]] inline const Block& block(const T& dimensions) const
    {
        auto index = (dimensions.z * BLOCKS_PER_Y_DIMENSION * BLOCKS_PER_X_DIMENSION) +
                     (dimensions.y * BLOCKS_PER_X_DIMENSION) + dimensions.x;
        return mBlocks[index];
    }

    template<typename T>
    inline Block& block(T x, T y, T z)
    {
        auto index = (z * BLOCKS_PER_Y_DIMENSION * BLOCKS_PER_X_DIMENSION) +
                     (y * BLOCKS_PER_X_DIMENSION) + x;
        return mBlocks[index];
    }

    template<typename T>
    [[nodiscard]] inline const Block& block(T x, T y, T z) const
    {
        auto index = (z * BLOCKS_PER_Y_DIMENSION * BLOCKS_PER_X_DIMENSION) +
                     (y * BLOCKS_PER_X_DIMENSION) + x;
        return mBlocks[index];
    }

private:
    std::array<Block, BLOCKS_IN_CHUNK> mBlocks;
};


class ChunkBlocksIterator
{
public:
    ChunkBlocksIterator(std::array<Block, ChunkBlocks::BLOCKS_IN_CHUNK>& blocks, int currentIndex)
        : mBlocks(blocks)
        , index(currentIndex)
    {
    }

    bool operator!=(const ChunkBlocksIterator& other) const
    {
        return index != other.index;
    }

    ChunkBlocksIterator& operator++()
    {
        ++index;
        return *this;
    }

    std::tuple<Block::Coordinate, Block&> operator*() const
    {
        auto z =
            index / (ChunkBlocks::BLOCKS_PER_X_DIMENSION * ChunkBlocks::BLOCKS_PER_Y_DIMENSION);
        auto y =
            (index / ChunkBlocks::BLOCKS_PER_X_DIMENSION) % ChunkBlocks::BLOCKS_PER_Y_DIMENSION;
        auto x = index % ChunkBlocks::BLOCKS_PER_X_DIMENSION;
        Block::Coordinate position{x, y, z};
        return {position, mBlocks[index]};
    }

private:
    std::array<Block, ChunkBlocks::BLOCKS_IN_CHUNK>& mBlocks;
    int index{0};
};

class ConstChunkBlocksIterator
{
public:
    ConstChunkBlocksIterator(const std::array<Block, ChunkBlocks::BLOCKS_IN_CHUNK>& blocks,
                             int currentIndex)
        : mBlocks(blocks)
        , index(currentIndex)
    {
    }

    bool operator!=(const ConstChunkBlocksIterator& other) const
    {
        return index != other.index;
    }

    ConstChunkBlocksIterator& operator++()
    {
        ++index;
        return *this;
    }

    std::tuple<Block::Coordinate, const Block&> operator*() const
    {
        auto z =
            index / (ChunkBlocks::BLOCKS_PER_X_DIMENSION * ChunkBlocks::BLOCKS_PER_Y_DIMENSION);
        auto y =
            (index / ChunkBlocks::BLOCKS_PER_X_DIMENSION) % ChunkBlocks::BLOCKS_PER_Y_DIMENSION;
        auto x = index % ChunkBlocks::BLOCKS_PER_X_DIMENSION;
        Block::Coordinate position{x, y, z};
        return {position, mBlocks[index]};
    }

private:
    const std::array<Block, ChunkBlocks::BLOCKS_IN_CHUNK>& mBlocks;
    int index{0};
};


class ChunkBlocksCacheUnfriendly
{
public:
    static constexpr auto BLOCKS_PER_X_DIMENSION = ChunkBlocks::BLOCKS_PER_X_DIMENSION;
    static constexpr auto BLOCKS_PER_Y_DIMENSION = ChunkBlocks::BLOCKS_PER_Y_DIMENSION;
    static constexpr auto BLOCKS_PER_Z_DIMENSION = ChunkBlocks::BLOCKS_PER_Z_DIMENSION;
    static constexpr auto BLOCKS_IN_CHUNK =
        BLOCKS_PER_X_DIMENSION * BLOCKS_PER_Y_DIMENSION * BLOCKS_PER_Z_DIMENSION;

    Block& block(glm::ivec3 dimensions)
    {
        return mBlocks[dimensions.x][dimensions.y][dimensions.z];
    }

    [[nodiscard]] const Block& block(glm::ivec3 dimensions) const
    {
        return mBlocks[dimensions.x][dimensions.y][dimensions.z];
    }


private:
    using ChunkBlocks = MultiDimensionalArray<Block, BLOCKS_PER_X_DIMENSION, BLOCKS_PER_Y_DIMENSION,
                                              BLOCKS_PER_Z_DIMENSION>;

    ChunkBlocks mBlocks;
};

}// namespace Voxino