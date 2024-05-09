#pragma once
#include "Renderer/Renderer.h"
#include "Resources/TexturePackArray.h"
#include "World/Camera.h"
#include "World/Chunks/ChunkBlocks.h"
#include "World/Chunks/ChunkContainerBase.h"

namespace Voxino
{
/**
 * \brief A container of chunks to manage them easily.
 *
 * A chunk container that stores chunks and can also create new
 * chunks inside itself - including on camera. It removes chunks
 * too far from the camera.
 */
template<typename ChunkType>
class ChunkContainer : public ChunkContainerBase
{
public:
    using Chunks = std::unordered_map<ChunkContainerBase::Coordinate, std::shared_ptr<ChunkType>,
                                      std::hash<CoordinateBase>>;

    ChunkContainer(const TexturePackArray& texturePackArray)
        : mTexturePackArray(texturePackArray)
    {
    }


    /**
     * \brief Draws the terrain of all chunks in the container.
     * \param renderer Renderer drawing the 3D game world onto the 2D screen
     * \param shader Shader with the help of which the object should be drawn
     * \param camera Camera through which the game world is viewed
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const override;

    /**
     * \brief Updates the chunkcontainer logic dependent, or independent of time, every rendered
     * frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime) override;

    void updateImGui() override;

    /**
     * \brief Finds a block inside a container based on the global position of the block
     * \param worldBlockCoordinates Global position of the block inside the game world
     * \return Pointer to block found, or nullptr if not found
     */
    [[nodiscard]] const Block* worldBlock(
        const Block::Coordinate& worldBlockCoordinates) const override;

    /**
     * \brief Finds a block inside a container based on the global position of the block
     * \param worldBlockCoordinates Global position of the block inside the game world
     * \return Pointer to block found, or nullptr if not found
     */
    [[nodiscard]] Block* worldBlock(const Block::Coordinate& worldBlockCoordinates) override;

    /**
     * \brief Returns information about whether a block on a given position has been already created
     * \param worldBlockCoordinates World coordinates of the block
     * \return True if such a block exists, false otherwise
     */
    [[nodiscard]] bool doesWorldBlockExist(
        const Block::Coordinate& worldBlockCoordinates) const override;

    /**
     * \brief Converts a block in the indicated position into an air block
     * \param worldBlockCoordinates World coordinates of the block to change
     */
    void removeWorldBlock(const Block::Coordinate& worldBlockCoordinates) override;

    /**
     * \brief Based on the position of the block in the game world, it returns the chunk that
     * contains it. \param worldBlockCoordinates Block coordinates in the game world \return Chunk,
     * which contains this block. Nullptr if the block is not present.
     */
    [[nodiscard]] std::shared_ptr<ChunkType> blockPositionToChunk(
        const Block::Coordinate& worldBlockCoordinates);

    /**
     * Returns the chunk located in the listed direction from this chunk
     * @param direction Direction next to which the chunk you are looking for is located
     * @return Pointer to chunk found
     */
    [[nodiscard]] std::shared_ptr<ChunkType> chunkNearby(const ChunkType& baseChunk,
                                                         const Direction& direction);

    /**
     * @brief Checks if the chunk is inside the container.
     */
    bool isChunkPresentInTheContainer(const ChunkType& chunk) const;

    /**
     * @brief Returns the map in which the chunks are located.
     */
    Chunks& data();

    /**
     * @brief Returns the map in which the chunks are located.
     */
    const Chunks& data() const;

    /**
     * @brief Finds the given chunk in the data and returns its shared_ptr. Useful in situations
     * where, with multithreading, the given chunk could be deleted and the reference to it would
     * become unsafe.
     * @param chunk Reference for the chunk to look for.
     * @return Shared_ptr to the chunk in the container, or nullptr if the chunk is not in the
     * container.
     */
    std::shared_ptr<ChunkType> findChunk(const ChunkType& chunk);

    /**
     * @brief Returns a chunk on a given block.
     * @param chunkCoordinate Coordinate the chunk to get.
     * @return Pointer to chunk with given coordinates
     */
    std::shared_ptr<ChunkType> at(const ChunkContainerBase::Coordinate& chunkCoordinate);

    /**
     * @brief Erases the chunk with the indicated coordinates
     * @param chunkCoordinate Coordinate the chunk to erase.
     * @return
     */
    std::size_t erase(const ChunkContainerBase::Coordinate& chunkCoordinate) override;

    /**
     * @brief Constructs a chunk based on the given parameters
     * @tparam Ts Types of parameters of the chunk constructor
     * @param args Parameters of the chunk constructor.
     * @return Returns a pair consisting of an iterator to the inserted element, or the
     * already-existing element if no insertion happened, and a bool denoting whether the insertion
     * took place (true if insertion happened, false if it did not).
     */
    template<class... Ts>
    auto emplace(Ts&&... args)
    {
        return data().emplace(std::forward<Ts>(args)...);
    }

    /**
     * @brief Returns whether a given chunk exists in the container
     * @param chunkPosition Position of the chunk sought
     * @return True if the object is in a container, false otherwise.
     */
    bool isPresent(const ChunkContainerBase::Coordinate& chunkPosition) const override;

    /**
     * @brief It insert a block in a chunk with rebuilding the chunk. If there was another block
     * then it is overwritten.
     * @param id Block id
     * @param worldCoordinate World coordinates of the block to place
     */
    void tryToPlaceBlock(const BlockId& id, Block::Coordinate worldCoordinate,
                         std::vector<BlockId> blocksThatMightBeOverplaced) override;

    /**
     * @brief Returns information about whether the container is empty, that is, whether it does not
     * contain any chunks.
     * @return True if the container is empty, false otherwise.
     */
    bool isEmpty() const override;

    /**
     * @brief Returns the number of chunks that are in the container.
     * @return Number of chunks that are in the container.
     */
    std::size_t size() const override;

private:
    /**
     * \brief Based on the position of the block in the game world, it returns the chunk that
     * contains it. \param worldBlockCoordinates Block coordinates in the game world \return Chunk,
     * which contains this block. Nullptr if the block is not present.
     */
    [[nodiscard]] std::shared_ptr<const ChunkType> blockPositionToChunk(
        const Block::Coordinate& worldBlockCoordinates) const;


private:
    /**
     * @brief Unordered map storing chunks inside this container.
     */
    Chunks mData;
    const TexturePackArray& mTexturePackArray;
};

template<typename ChunkType>
void ChunkContainer<ChunkType>::draw(const Renderer& renderer, const Shader& shader,
                                     const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    for (auto& [coordinate, chunk]: data())
    {
        chunk->draw(renderer, shader, camera);
    }
}

template<typename ChunkType>
const Block* ChunkContainer<ChunkType>::worldBlock(
    const Block::Coordinate& worldBlockCoordinates) const
{
    if (const auto chunk = blockPositionToChunk(worldBlockCoordinates))
    {
        return &chunk->localBlock(chunk->globalToLocalCoordinates(worldBlockCoordinates));
    }
    return nullptr;
}

template<typename ChunkType>
Block* ChunkContainer<ChunkType>::worldBlock(const Block::Coordinate& worldBlockCoordinates)
{
    return const_cast<Block*>(
        static_cast<const ChunkContainer&>(*this).worldBlock(worldBlockCoordinates));
}

template<typename ChunkType>
bool ChunkContainer<ChunkType>::doesWorldBlockExist(
    const Block::Coordinate& worldBlockCoordinates) const
{
    const auto foundChunk =
        data().find(ChunkContainerBase::Coordinate::blockToChunkMetric(worldBlockCoordinates));
    return foundChunk != data().cend();
}

template<typename ChunkType>
std::shared_ptr<const ChunkType> ChunkContainer<ChunkType>::blockPositionToChunk(
    const Block::Coordinate& worldBlockCoordinates) const
{
    const auto foundChunk =
        data().find(ChunkContainerBase::Coordinate::blockToChunkMetric(worldBlockCoordinates));
    if (foundChunk != data().cend())
    {
        return foundChunk->second;
    }
    return nullptr;
}

template<typename ChunkType>
std::shared_ptr<ChunkType> ChunkContainer<ChunkType>::blockPositionToChunk(
    const Block::Coordinate& worldBlockCoordinates)
{
    return std::const_pointer_cast<ChunkType>(
        static_cast<const ChunkContainer&>(*this).blockPositionToChunk(worldBlockCoordinates));
}


template<typename ChunkType>
void ChunkContainer<ChunkType>::removeWorldBlock(const Block::Coordinate& worldBlockCoordinates)
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
                // neighboringChunk->rebuildMesh(); // TODO
            }
        }
    }
}

template<typename ChunkType>
std::shared_ptr<ChunkType> ChunkContainer<ChunkType>::chunkNearby(const ChunkType& baseChunk,
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
                baseChunk.localToGlobalCoordinates({ChunkBlocks::BLOCKS_PER_X_DIMENSION, 0, 0}));
        }
        case Direction::InFront:
        {
            return blockPositionToChunk(
                baseChunk.localToGlobalCoordinates({0, 0, ChunkBlocks::BLOCKS_PER_Z_DIMENSION}));
        }
        case Direction::Behind:
        {
            return blockPositionToChunk(baseChunk.localToGlobalCoordinates({0, 0, -1}));
        }
        case Direction::Above:
        {
            return blockPositionToChunk(
                baseChunk.localToGlobalCoordinates({0, ChunkBlocks::BLOCKS_PER_Y_DIMENSION, 0}));
        }
        case Direction::Below:
        {
            return blockPositionToChunk(baseChunk.localToGlobalCoordinates({0, -1, 0}));
        }
        default: throw std::runtime_error("Unsupported Direction value was provided");
    }
}

template<typename ChunkType>
typename ChunkContainer<ChunkType>::Chunks& ChunkContainer<ChunkType>::data()
{
    return mData;
}

template<typename ChunkType>
const typename ChunkContainer<ChunkType>::Chunks& ChunkContainer<ChunkType>::data() const
{
    return mData;
}

template<typename ChunkType>
bool ChunkContainer<ChunkType>::isChunkPresentInTheContainer(const ChunkType& chunk) const
{
    return data().find(ChunkContainerBase::Coordinate::blockToChunkMetric(
               chunk.positionInBlocks())) != data().cend();
}

template<typename ChunkType>
std::shared_ptr<ChunkType> ChunkContainer<ChunkType>::findChunk(const ChunkType& chunk)
{
    auto foundChunk =
        data().find(ChunkContainerBase::Coordinate::blockToChunkMetric(chunk.positionInBlocks()));

    return (foundChunk != data().end()) ? foundChunk->second : nullptr;
}

template<typename ChunkType>
std::shared_ptr<ChunkType> ChunkContainer<ChunkType>::at(
    const ChunkContainerBase::Coordinate& chunkCoordinate)
{
    return data().at(chunkCoordinate);
}

template<typename ChunkType>
std::size_t ChunkContainer<ChunkType>::erase(const ChunkContainerBase::Coordinate& chunkCoordinate)
{
    return data().erase(chunkCoordinate);
}

template<typename ChunkType>
bool ChunkContainer<ChunkType>::isPresent(const ChunkContainerBase::Coordinate& chunkPosition) const
{
    return data().find(chunkPosition) != data().end();
}

template<typename ChunkType>
void ChunkContainer<ChunkType>::tryToPlaceBlock(const BlockId& id,
                                                Block::Coordinate worldCoordinate,
                                                std::vector<BlockId> blocksThatMightBeOverplaced)
{
    if (const auto chunk = blockPositionToChunk(worldCoordinate))
    {
        auto localChunkCoordinates = chunk->globalToLocalCoordinates(worldCoordinate);
        chunk->tryToPlaceBlock(id, localChunkCoordinates, blocksThatMightBeOverplaced);
    }
}

template<typename ChunkType>
bool ChunkContainer<ChunkType>::isEmpty() const
{
    return mData.empty();
}

template<typename ChunkType>
std::size_t ChunkContainer<ChunkType>::size() const
{
    return mData.size();
}

template<typename ChunkType>
void ChunkContainer<ChunkType>::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    for (auto& [coordinate, chunk]: data())
    {
        chunk->update(deltaTime);
    }
}

template<typename ChunkType>
void ChunkContainer<ChunkType>::updateImGui()
{
    MEASURE_SCOPE;
    for (auto& [coordinate, chunk]: data())
    {
        chunk->updateImGui();
    }
}

}// namespace Voxino