#include "ChunkContainerBase.h"
#include "World/Chunks/ChunkBlocks.h"

namespace Voxino
{

sf::Vector3i ChunkContainerBase::Coordinate::nonChunkMetric() const
{
    return sf::Vector3i(x * ChunkBlocks::BLOCKS_PER_X_DIMENSION * Block::BLOCK_SIZE,
                        y * ChunkBlocks::BLOCKS_PER_Y_DIMENSION * Block::BLOCK_SIZE,
                        z * ChunkBlocks::BLOCKS_PER_Z_DIMENSION * Block::BLOCK_SIZE);
}

ChunkContainerBase::Coordinate ChunkContainerBase::Coordinate::blockToChunkMetric(
    const Block::Coordinate& worldBlockCoordinate)
{

    static auto fastFloor = [](const float& numberToFloor)
    {
        const auto truncated = static_cast<int>(numberToFloor);
        return truncated - (truncated > numberToFloor);
    };

    // clang-format off
    auto returnVar = ChunkContainerBase::Coordinate(
        fastFloor(worldBlockCoordinate.x / static_cast<float>(ChunkBlocks::BLOCKS_PER_X_DIMENSION)),
        fastFloor(worldBlockCoordinate.y / static_cast<float>(ChunkBlocks::BLOCKS_PER_Y_DIMENSION)),
        fastFloor(worldBlockCoordinate.z / static_cast<float>(ChunkBlocks::BLOCKS_PER_Z_DIMENSION))
    );
    // clang-format on

    return returnVar;
}

}// namespace Voxino