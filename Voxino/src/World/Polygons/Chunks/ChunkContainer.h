#pragma once
#include "Renderer/Renderer.h"
#include "World/Camera.h"
#include "World/Polygons/Chunks/ChunkInterface.h"


#define DRAW_DEBUG_COLLISIONS false

#if DRAW_DEBUG_COLLISIONS
    #include <Physics/AABB.h>
    #include <deque>
#endif

namespace Voxino
{
/**
 * \brief A container of chunks to manage them easily.
 *
 * A chunk container that stores chunks and can also create new
 * chunks inside itself - including on camera. It removes chunks
 * too far from the camera.
 */
class ChunkContainer
{
public:
    struct Coordinate final : public CoordinateBase
    {

        using CoordinateBase::CoordinateBase;

        /**
         * Transforms chunk coordinates to game world coordinates.
         * @return Coordinates in the position of the game world
         */
        [[nodiscard]] sf::Vector3i nonChunkMetric() const;

        /**
         * Converts the coordinates of the block to the coordinates of the chunk in which it is
         * located. These coordinates are chunk-specific and independent of the game world
         * coordinates.
         *
         * @param worldBlockCoordinate World coordinates of the block
         * @return Chunk coordinates
         */
        static ChunkContainer::Coordinate blockToChunkMetric(
            const Block::Coordinate& worldBlockCoordinate);
    };

    using Chunks = std::unordered_map<ChunkContainer::Coordinate, std::shared_ptr<ChunkInterface>,
                                      std::hash<CoordinateBase>>;

    ChunkContainer() = default;

    /**
     * Draws terrain of chunks in the container to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawTerrain(const Renderer& renderer, const Shader& shader, const Camera& camera) const;

    /**
     * Draws liquids of chunks in the container to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawLiquids(const Renderer& renderer, const Shader& shader, const Camera& camera) const;

    /**
     * Draws florals of chunks in the container to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawFlorals(const Renderer& renderer, const Shader& shader, const Camera& camera) const;

    /**
     * \brief Updates the chunkcontainer logic dependent, or independent of time, every rendered
     * frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    void update(const float& deltaTime);

    /**
     * \brief Finds a block inside a container based on the global position of the block
     * \param worldBlockCoordinates Global position of the block inside the game world
     * \return Pointer to block found, or nullptr if not found
     */
    [[nodiscard]] const Block* worldBlock(const Block::Coordinate& worldBlockCoordinates) const;

    /**
     * \brief Finds a block inside a container based on the global position of the block
     * \param worldBlockCoordinates Global position of the block inside the game world
     * \return Pointer to block found, or nullptr if not found
     */
    [[nodiscard]] Block* worldBlock(const Block::Coordinate& worldBlockCoordinates);

    /**
     * \brief Returns information about whether a block on a given position has been already created
     * \param worldBlockCoordinates World coordinates of the block
     * \return True if such a block exists, false otherwise
     */
    [[nodiscard]] bool doesWorldBlockExist(const Block::Coordinate& worldBlockCoordinates) const;

    /**
     * \brief Converts a block in the indicated position into an air block
     * \param worldBlockCoordinates World coordinates of the block to change
     */
    void removeWorldBlock(const Block::Coordinate& worldBlockCoordinates);

    /**
     * \brief Based on the position of the block in the game world, it returns the chunk that
     * contains it. \param worldBlockCoordinates Block coordinates in the game world \return Chunk,
     * which contains this block. Nullptr if the block is not present.
     */
    [[nodiscard]] std::shared_ptr<ChunkInterface> blockPositionToChunk(
        const Block::Coordinate& worldBlockCoordinates);

    /**
     * Returns the chunk located in the listed direction from this chunk
     * @param direction Direction next to which the chunk you are looking for is located
     * @return Pointer to chunk found
     */
    [[nodiscard]] std::shared_ptr<ChunkInterface> chunkNearby(const ChunkInterface& baseChunk,
                                                              const Direction& direction);

    /**
     * @brief Rebuilds chunks around a given chunk.
     * @param chunkCoordinates Coordinates chunk around which other chunks should be rebuilt.
     */
    void rebuildChunksAround(ChunkContainer::Coordinate chunkCoordinates);

    /**
     * @brief Checks if the chunk is inside the container.
     */
    bool isChunkPresentInTheContainer(const ChunkInterface& chunk) const;

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
    std::shared_ptr<ChunkInterface> findChunk(const ChunkInterface& chunk);

    /**
     * @brief Returns a chunk on a given block.
     * @param chunkCoordinate Coordinate the chunk to get.
     * @return Pointer to chunk with given coordinates
     */
    std::shared_ptr<ChunkInterface> at(const ChunkContainer::Coordinate& chunkCoordinate);

    /**
     * @brief Erases the chunk with the indicated coordinates
     * @param chunkCoordinate Coordinate the chunk to erase.
     * @return
     */
    Chunks::size_type erase(const ChunkContainer::Coordinate& chunkCoordinate);

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
    bool isPresent(const ChunkContainer::Coordinate& chunkPosition) const;

    /**
     * @brief It insert a block in a chunk with rebuilding the chunk. If there was another block
     * then it is overwritten.
     * @param id Block id
     * @param worldCoordinate World coordinates of the block to place
     */
    void tryToPlaceBlock(const BlockId& id, Block::Coordinate worldCoordinate,
                         std::vector<BlockId> blocksThatMightBeOverplaced);

    /**
     * @brief Returns information about whether the container is empty, that is, whether it does not
     * contain any chunks.
     * @return True if the container is empty, false otherwise.
     */
    bool isEmpty() const;

    /**
     * @brief Returns the number of chunks that are in the container.
     * @return Number of chunks that are in the container.
     */
    std::size_t size() const;

private:
    /**
     * \brief Based on the position of the block in the game world, it returns the chunk that
     * contains it. \param worldBlockCoordinates Block coordinates in the game world \return Chunk,
     * which contains this block. Nullptr if the block is not present.
     */
    [[nodiscard]] std::shared_ptr<const ChunkInterface> blockPositionToChunk(
        const Block::Coordinate& worldBlockCoordinates) const;

    /**
     * @brief It checks if a new chunk appeared that has queued blocks to be inserted instead of air
     * blocks.
     */
    void tryToPlaceScheduledBlocksForPresentChunks();

    /**
     * @brief It checks if a new chunk appeared that has queued blocks to be inserted instead of air
     * blocks.
     */
    void tryToPlaceScheduledBlocksForNewAppearingChunks();


private:
    /**
     * @brief Stores information on a block to appear in one of the newly created chunks in the
     * future
     */
    struct BlockToBePlaced
    {
        ChunkContainer::Coordinate chunkCoordinates;
        BlockId blockid;
        Block::Coordinate worldBlockCoordinates;
        std::vector<BlockId> blocksThatMightBeOverplaced;
    };

    /**
     * @brief A queue of blocks that might replace air block with new block in a chunk that has not
     * yet been created.
     */
    std::list<BlockToBePlaced> mBlockMightBePlacedInFutureChunks;

    /**
     * @brief Unordered map storing chunks inside this container.
     */
    Chunks mData;
};

}// namespace Voxino