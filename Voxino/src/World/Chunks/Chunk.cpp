#include "Chunk.h"
#include "World/Block/Block.h"
#include "World/Chunks/ChunkContainer.h"
#include "World/Chunks/SimpleTerrainGenerator.h"
#include "pch.h"


namespace Voxino
{

// Chunk::Chunk(Block::Coordinate blockPosition, const TexturePackArray& texturePack,
//              ChunkContainer& parent)
//     : mChunkPosition(std::move(blockPosition))
//     , mTexturePack(texturePack)
//     , mParentContainer(&parent)
//     , mChunkOfBlocks(std::make_unique<ChunkBlocks>())
//     , mTerrainGenerator(std::make_unique<SimpleTerrainGenerator>())
// {
//     auto chunkCoordinate = ChunkContainer::Coordinate::blockToChunkMetric(mChunkPosition);
//     generateChunkTerrain();
// } // TODO

Chunk::Chunk(Block::Coordinate blockPosition, const TexturePackArray& texturePack)
    : mChunkPosition(std::move(blockPosition))
    , mTexturePack(texturePack)
    // , mParentContainer()
    , mChunkOfBlocks(std::make_unique<ChunkBlocks>())
    , mTerrainGenerator(std::make_unique<SimpleTerrainGenerator>())
{
    auto chunkCoordinate = ChunkContainer::Coordinate::blockToChunkMetric(mChunkPosition);
    generateChunkTerrain();
}

Chunk::Chunk(Chunk&& rhs) noexcept
    : mChunkPosition(std::move(rhs.mChunkPosition))
    , mTexturePack(rhs.mTexturePack)
    // , mParentContainer(rhs.mParentContainer)
    // , mTerrainModel(std::move(rhs.mTerrainModel)) // TODO
    , mChunkOfBlocks(std::move(rhs.mChunkOfBlocks))
    , mTerrainGenerator(std::move(rhs.mTerrainGenerator))
{
}


bool Chunk::areLocalCoordinatesInsideChunk(const Block::Coordinate& localCoordinates)
{
    if (localCoordinates.x < ChunkBlocks::BLOCKS_PER_X_DIMENSION && localCoordinates.x >= 0 &&
        localCoordinates.y < ChunkBlocks::BLOCKS_PER_Y_DIMENSION && localCoordinates.y >= 0 &&
        localCoordinates.z < ChunkBlocks::BLOCKS_PER_Z_DIMENSION && localCoordinates.z >= 0)
    {
        return true;
    }

    return false;
}

bool Chunk::isLocalCoordinateOnChunkEdge(const Block::Coordinate& localCoordinates)
{
    if (localCoordinates.x == ChunkBlocks::BLOCKS_PER_X_DIMENSION - 1 || localCoordinates.x == 0 ||
        localCoordinates.y == ChunkBlocks::BLOCKS_PER_Y_DIMENSION - 1 || localCoordinates.y == 0 ||
        localCoordinates.z == ChunkBlocks::BLOCKS_PER_Z_DIMENSION - 1 || localCoordinates.z == 0)
    {
        return true;
    }

    return false;
}

void Chunk::removeLocalBlock(const Block::Coordinate& localCoordinates)
{
    mChunkOfBlocks->block(localCoordinates).setBlockType(BlockId::Air);

    // rebuildMesh(); // TODO
}

Block::Coordinate Chunk::globalToLocalCoordinates(const Block::Coordinate& worldCoordinates) const
{
    return static_cast<Block::Coordinate>(worldCoordinates - mChunkPosition);
}

void Chunk::fixedUpdate(const float& deltaTime)
{
}

Block& Chunk::localBlock(const Block::Coordinate& localCoordinates)
{
    return const_cast<Block&>(static_cast<const Chunk&>(*this).localBlock(localCoordinates));
}

Block& Chunk::localNearbyBlock(const Block::Coordinate& position, const Direction& direction)
{
    return const_cast<Block&>(
        static_cast<const Chunk&>(*this).localNearbyBlock(position, direction));
}

const Block& Chunk::localBlock(const Block::Coordinate& localCoordinates) const
{
    return mChunkOfBlocks->block(localCoordinates);
}

const Block& Chunk::localNearbyBlock(const Block::Coordinate& localCoordinates,
                                     const Direction& direction) const
{
    return localBlock(localNearbyBlockPosition(localCoordinates, direction));
}

Block::Coordinate Chunk::localNearbyBlockPosition(const Block::Coordinate& position,
                                                  const Direction& direction) const
{
    switch (direction)
    {
        case Direction::Above: return {position.x, position.y + 1, position.z};
        case Direction::Below: return {position.x, position.y - 1, position.z};
        case Direction::ToTheLeft: return {position.x - 1, position.y, position.z};
        case Direction::ToTheRight: return {position.x + 1, position.y, position.z};
        case Direction::InFront: return {position.x, position.y, position.z + 1};
        case Direction::Behind: return {position.x, position.y, position.z - 1};
        default: throw std::runtime_error("Unsupported Direction value was provided");
    }
}

Block::Coordinate Chunk::localToGlobalCoordinates(const Block::Coordinate& localCoordinates) const
{
    return static_cast<Block::Coordinate>(mChunkPosition + localCoordinates);
}

std::vector<Direction> Chunk::directionOfBlockFacesInContactWithOtherChunk(
    const Block::Coordinate& localCoordinates)
{
    std::vector<Direction> directions;

    if (localCoordinates.x == ChunkBlocks::BLOCKS_PER_X_DIMENSION - 1)
    {
        directions.emplace_back(Direction::ToTheRight);
    }
    if (localCoordinates.x == 0)
    {
        directions.emplace_back(Direction::ToTheLeft);
    }
    if (localCoordinates.y == ChunkBlocks::BLOCKS_PER_Y_DIMENSION - 1)
    {
        directions.emplace_back(Direction::Above);
    }
    if (localCoordinates.y == 0)
    {
        directions.emplace_back(Direction::Below);
    }
    if (localCoordinates.z == ChunkBlocks::BLOCKS_PER_Z_DIMENSION - 1)
    {
        directions.emplace_back(Direction::InFront);
    }
    if (localCoordinates.z == 0)
    {
        directions.emplace_back(Direction::Behind);
    }

    return directions;
}

bool Chunk::doesBlockFaceHasTransparentNeighbor(const Block::Face& blockFace,
                                                const Block::Coordinate& blockPos)
{
    auto isBlockTransparent = [&blockPos, this](const Direction& face)
    {
        auto neighbour = neighbourBlockInGivenDirection(blockPos, face);
        return (not neighbour.has_value()) or
               (neighbour.has_value() && neighbour.value().isTransparent());
    };

    switch (blockFace)
    {
        case Block::Face::Top: return (isBlockTransparent(Direction::Above));
        case Block::Face::Left: return (isBlockTransparent(Direction::ToTheLeft));
        case Block::Face::Right: return (isBlockTransparent(Direction::ToTheRight));
        case Block::Face::Bottom: return (isBlockTransparent(Direction::Below));
        case Block::Face::Front: return (isBlockTransparent(Direction::InFront));
        case Block::Face::Back: return (isBlockTransparent(Direction::Behind));
        default: throw std::runtime_error("Unsupported Block::Face value was provided");
    }
}

bool Chunk::doesBlockFaceHasGivenBlockNeighbour(const Block::Face& blockFace,
                                                const Block::Coordinate& blockPos,
                                                const BlockId& blockId)
{
    auto isBlockOfGivenId = [&blockPos, &blockId, this](const Direction& face)
    {
        auto neighbour = neighbourBlockInGivenDirection(blockPos, face);
        return (neighbour.has_value() && neighbour.value().id() == blockId);
    };

    switch (blockFace)
    {
        case Block::Face::Top: return (isBlockOfGivenId(Direction::Above));
        case Block::Face::Left: return (isBlockOfGivenId(Direction::ToTheLeft));
        case Block::Face::Right: return (isBlockOfGivenId(Direction::ToTheRight));
        case Block::Face::Bottom: return (isBlockOfGivenId(Direction::Below));
        case Block::Face::Front: return (isBlockOfGivenId(Direction::InFront));
        case Block::Face::Back: return (isBlockOfGivenId(Direction::Behind));
        default: throw std::runtime_error("Unsupported Block::Face value was provided");
    }
}

std::optional<Block> Chunk::neighbourBlockInGivenDirection(const Block::Coordinate& blockPos,
                                                           const Direction& direction)
{
    const auto blockNeighborPosition = localNearbyBlockPosition(blockPos, direction);
    if (areLocalCoordinatesInsideChunk(blockNeighborPosition))
    {
        return std::optional<Block>(localBlock(blockNeighborPosition).id());
    }

    // if (mParentContainer)
    // {
    //     if (const auto& neighborBlock =
    //             mParentContainer->worldBlock(localToGlobalCoordinates(blockNeighborPosition)))
    //     {
    //         return std::optional<Block>(neighborBlock->id());
    //     }
    // } // TODO

    return std::nullopt;
}

const Block::Coordinate& Chunk::positionInBlocks() const
{
    return mChunkPosition;
}

bool Chunk::tryToPlaceBlock(const BlockId& blockId, const Block::Coordinate& localCoordinates,
                            std::vector<BlockId> blocksThatMightBeOverplaced)
{
    MEASURE_SCOPE;
    if (areLocalCoordinatesInsideChunk(localCoordinates))
    {
        return tryToPlaceBlockInsideThisChunk(blockId, localCoordinates,
                                              blocksThatMightBeOverplaced);
    }
    // else if (mParentContainer)
    // {
    //     auto globalCoordinates = localToGlobalCoordinates(localCoordinates);
    //     mParentContainer->tryToPlaceBlock(blockId, globalCoordinates,
    //     blocksThatMightBeOverplaced);
    // } // TODO
    return false;
}

bool Chunk::tryToPlaceBlockInsideThisChunk(const BlockId& blockId,
                                           const Block::Coordinate& localCoordinates,
                                           std::vector<BlockId>& blocksThatMightBeOverplaced)
{
    auto& block = mChunkOfBlocks->block(localCoordinates);
    auto idOfTheBlockToOverplace = block.id();

    if (canGivenBlockBeOverplaced(blocksThatMightBeOverplaced, idOfTheBlockToOverplace))
    {
        block.setBlockType(blockId);
        // rebuildMesh(); // TODO
        return true;
    }
    return false;
}

void Chunk::generateChunkTerrain()
{
    MEASURE_SCOPE;
    mTerrainGenerator->generateTerrain(*this, *mChunkOfBlocks);
}

bool Chunk::canGivenBlockBeOverplaced(std::vector<BlockId>& blocksThatMightBeOverplaced,
                                      const BlockId& idOfTheBlockToOverplace) const
{
    return std::any_of(blocksThatMightBeOverplaced.begin(), blocksThatMightBeOverplaced.end(),
                       [&idOfTheBlockToOverplace](const auto& blockThatMightBeOverplaced)
                       {
                           return (blockThatMightBeOverplaced == idOfTheBlockToOverplace) ||
                                  (blockThatMightBeOverplaced == BlockId::AllBlocks);
                       });
}

}// namespace Voxino