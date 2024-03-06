#include "ChunkContainer.h"
#include "CoordinatesAroundOriginGetter.h"
#include "Utils/IteratorRanges.h"
#include "World/Polygons/Block/BlockMap.h"
#include "pch.h"

namespace Voxino
{

void ChunkContainer::drawTerrain(const Renderer& renderer, const Shader& shader,
                                 const Camera& camera) const
{
    for (auto& [coordinate, chunk]: data())
    {
        chunk->drawTerrain(renderer, shader, camera);
    }
}

void ChunkContainer::drawLiquids(const Renderer& renderer, const Shader& shader,
                                 const Camera& camera) const
{
    ;
    for (auto& [coordinate, chunk]: data())
    {
        chunk->drawLiquids(renderer, shader, camera);
    }
}

void ChunkContainer::drawFlorals(const Renderer& renderer, const Shader& shader,
                                 const Camera& camera) const
{
    for (auto& [coordinate, chunk]: data())
    {
        chunk->drawFlorals(renderer, shader, camera);
    }
}

void ChunkContainer::update(const float& deltaTime)
{
    tryToPlaceScheduledBlocksForNewAppearingChunks();
}

void ChunkContainer::tryToPlaceScheduledBlocksForNewAppearingChunks()
{
    if (!mBlockMightBePlacedInFutureChunks.empty())
    {
        tryToPlaceScheduledBlocksForPresentChunks();
    }
}

void ChunkContainer::tryToPlaceScheduledBlocksForPresentChunks()
{
    for (auto [begIter, endIter] = iterator_range(mBlockMightBePlacedInFutureChunks);
         begIter != endIter;)
    {
        auto& blockToBePlaced = *begIter;
        if (isPresent(blockToBePlaced.chunkCoordinates))
        {
            auto chunk = blockPositionToChunk(blockToBePlaced.worldBlockCoordinates);
            auto blockLocalCoordinates =
                chunk->globalToLocalCoordinates(blockToBePlaced.worldBlockCoordinates);

            chunk->tryToPlaceBlock(blockToBePlaced.blockid, blockLocalCoordinates,
                                   blockToBePlaced.blocksThatMightBeOverplaced);

            begIter = mBlockMightBePlacedInFutureChunks.erase(begIter);
        }
        else
        {
            ++begIter;
        }
    }
}

sf::Vector3i ChunkContainer::Coordinate::nonChunkMetric() const
{
    return sf::Vector3i(x * ChunkInterface::BLOCKS_PER_X_DIMENSION * Block::BLOCK_SIZE,
                        y * ChunkInterface::BLOCKS_PER_Y_DIMENSION * Block::BLOCK_SIZE,
                        z * ChunkInterface::BLOCKS_PER_Z_DIMENSION * Block::BLOCK_SIZE);
}

ChunkContainer::Coordinate ChunkContainer::Coordinate::blockToChunkMetric(
    const Block::Coordinate& worldBlockCoordinate)
{

    static auto fastFloor = [](const float& numberToFloor)
    {
        const auto truncated = static_cast<int>(numberToFloor);
        return truncated - (truncated > numberToFloor);
    };

    // clang-format off
    auto returnVar = ChunkContainer::Coordinate(
        fastFloor(worldBlockCoordinate.x / static_cast<float>(ChunkInterface::BLOCKS_PER_X_DIMENSION)),
        fastFloor(worldBlockCoordinate.y / static_cast<float>(ChunkInterface::BLOCKS_PER_Y_DIMENSION)),
        fastFloor(worldBlockCoordinate.z / static_cast<float>(ChunkInterface::BLOCKS_PER_Z_DIMENSION))
    );
    // clang-format on

    return returnVar;
}

const Block* ChunkContainer::worldBlock(const Block::Coordinate& worldBlockCoordinates) const
{
    if (const auto chunk = blockPositionToChunk(worldBlockCoordinates))
    {
        return &chunk->localBlock(chunk->globalToLocalCoordinates(worldBlockCoordinates));
    }
    return nullptr;
}

Block* ChunkContainer::worldBlock(const Block::Coordinate& worldBlockCoordinates)
{
    return const_cast<Block*>(
        static_cast<const ChunkContainer&>(*this).worldBlock(worldBlockCoordinates));
}

bool ChunkContainer::doesWorldBlockExist(const Block::Coordinate& worldBlockCoordinates) const
{
    const auto foundChunk =
        data().find(ChunkContainer::Coordinate::blockToChunkMetric(worldBlockCoordinates));
    return foundChunk != data().cend();
}

std::shared_ptr<const ChunkInterface> ChunkContainer::blockPositionToChunk(
    const Block::Coordinate& worldBlockCoordinates) const
{
    const auto foundChunk =
        data().find(ChunkContainer::Coordinate::blockToChunkMetric(worldBlockCoordinates));
    if (foundChunk != data().cend())
    {
        return foundChunk->second;
    }
    return nullptr;
}

std::shared_ptr<ChunkInterface> ChunkContainer::blockPositionToChunk(
    const Block::Coordinate& worldBlockCoordinates)
{
    return std::const_pointer_cast<ChunkInterface>(
        static_cast<const ChunkContainer&>(*this).blockPositionToChunk(worldBlockCoordinates));
}


void ChunkContainer::removeWorldBlock(const Block::Coordinate& worldBlockCoordinates)
{
    if (const auto chunk = blockPositionToChunk(worldBlockCoordinates))
    {
        auto localCoordinates = chunk->globalToLocalCoordinates(worldBlockCoordinates);

        chunk->removeLocalBlock(localCoordinates);

        /*
         * When removing a block, you may find that it is in contact with an adjacent chunk.
         * Rebuilding one chunk doesn't help, because the neighboring chunk remains in the form
         * where it assumes the block is there. This leads to a hole in the chunk. For this reason,
         * must rebuild all chunks that are in contact with the block being removed.
         */
        const auto directions =
            chunk->directionOfBlockFacesInContactWithOtherChunk(localCoordinates);
        for (auto& blockDirection: directions)
        {
            auto neighboringBlockInOtherChunk = chunk->localToGlobalCoordinates(
                chunk->localNearbyBlockPosition(localCoordinates, blockDirection));

            if (const auto neighboringChunk = blockPositionToChunk(neighboringBlockInOtherChunk))
            {
                neighboringChunk->rebuildMesh();
            }
        }
    }
}

std::shared_ptr<ChunkInterface> ChunkContainer::chunkNearby(const ChunkInterface& baseChunk,
                                                            const Direction& direction)
{
    switch (direction)
    {
        case Direction::ToTheLeft:
        {
            return blockPositionToChunk(baseChunk.localToGlobalCoordinates({-1, 0, 0}));
        }
        case Direction::ToTheRight:
        {
            return blockPositionToChunk(
                baseChunk.localToGlobalCoordinates({ChunkInterface::BLOCKS_PER_X_DIMENSION, 0, 0}));
        }
        case Direction::InFront:
        {
            return blockPositionToChunk(
                baseChunk.localToGlobalCoordinates({0, 0, ChunkInterface::BLOCKS_PER_Z_DIMENSION}));
        }
        case Direction::Behind:
        {
            return blockPositionToChunk(baseChunk.localToGlobalCoordinates({0, 0, -1}));
        }
        default: throw std::runtime_error("Unsupported Direction value was provided");
    }
}

ChunkContainer::Chunks& ChunkContainer::data()
{
    return mData;
}

const ChunkContainer::Chunks& ChunkContainer::data() const
{
    return mData;
}

bool ChunkContainer::isChunkPresentInTheContainer(const ChunkInterface& chunk) const
{
    return data().find(ChunkContainer::Coordinate::blockToChunkMetric(chunk.positionInBlocks())) !=
           data().cend();
}

std::shared_ptr<ChunkInterface> ChunkContainer::findChunk(const ChunkInterface& chunk)
{
    auto foundChunk =
        data().find(ChunkContainer::Coordinate::blockToChunkMetric(chunk.positionInBlocks()));

    return (foundChunk != data().end()) ? foundChunk->second : nullptr;
}

void ChunkContainer::rebuildChunksAround(ChunkContainer::Coordinate chunkCoordinates)
{
    auto& chunk = data().at(chunkCoordinates);
    for (auto direction:
         {Direction::Behind, Direction::InFront, Direction::ToTheLeft, Direction::ToTheRight})
    {
        if (const auto chunkClose = chunkNearby(*chunk, direction))
        {
            chunkClose->rebuildMesh();
        }
    }
}
std::shared_ptr<ChunkInterface> ChunkContainer::at(
    const ChunkContainer::Coordinate& chunkCoordinate)
{
    return data().at(chunkCoordinate);
}

ChunkContainer::Chunks::size_type ChunkContainer::erase(
    const ChunkContainer::Coordinate& chunkCoordinate)
{
    return data().erase(chunkCoordinate);
}

bool ChunkContainer::isPresent(const ChunkContainer::Coordinate& chunkPosition) const
{
    return data().find(chunkPosition) != data().end();
}

void ChunkContainer::tryToPlaceBlock(const BlockId& id, Block::Coordinate worldCoordinate,
                                     std::vector<BlockId> blocksThatMightBeOverplaced)
{
    if (const auto chunk = blockPositionToChunk(worldCoordinate))
    {
        auto localChunkCoordinates = chunk->globalToLocalCoordinates(worldCoordinate);
        chunk->tryToPlaceBlock(id, localChunkCoordinates, blocksThatMightBeOverplaced);
    }
    else
    {
        const auto& chunkCoordinates =
            ChunkContainer::Coordinate::blockToChunkMetric(worldCoordinate);

        mBlockMightBePlacedInFutureChunks.push_back(
            BlockToBePlaced{chunkCoordinates, id, worldCoordinate, blocksThatMightBeOverplaced});
    }
}


bool ChunkContainer::isEmpty() const
{
    return mData.empty();
}

std::size_t ChunkContainer::size() const
{
    return mData.size();
}


}// namespace Voxino