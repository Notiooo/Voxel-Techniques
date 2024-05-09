#pragma once
#include "Renderer/Renderer.h"
#include "Utils/CoordinateBase.h"
#include "World/Block/Block.h"
#include "World/Camera.h"

namespace Voxino
{
class ChunkContainerBase
{
public:
    static constexpr auto MAX_CHUNKS_IN_HEIGHT = 4;
    static constexpr auto CHUNK_RADIUS = CHUNK_CONTAINER_RADIUS;
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
        static ChunkContainerBase::Coordinate blockToChunkMetric(
            const Block::Coordinate& worldBlockCoordinate);
    };

    virtual ~ChunkContainerBase() = default;

    /**
     * \brief Draws the terrain of all chunks in the container.
     * \param renderer Renderer drawing the 3D game world onto the 2D screen
     * \param shader Shader with the help of which the object should be drawn
     * \param camera Camera through which the game world is viewed
     */
    virtual void draw(const Renderer& renderer, const Shader& shader,
                      const Camera& camera) const = 0;

    /**
     * \brief Updates the ChunkContainerBase logic dependent, or independent of time, every rendered
     * frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    virtual void update(const float& deltaTime) = 0;

    virtual void updateImGui() = 0;

    /**
     * \brief Finds a block inside a container based on the global position of the block
     * \param worldBlockCoordinates Global position of the block inside the game world
     * \return Pointer to block found, or nullptr if not found
     */
    [[nodiscard]] virtual const Block* worldBlock(
        const Block::Coordinate& worldBlockCoordinates) const = 0;

    /**
     * \brief Finds a block inside a container based on the global position of the block
     * \param worldBlockCoordinates Global position of the block inside the game world
     * \return Pointer to block found, or nullptr if not found
     */
    [[nodiscard]] virtual Block* worldBlock(const Block::Coordinate& worldBlockCoordinates) = 0;

    /**
     * \brief Returns information about whether a block on a given position has been already created
     * \param worldBlockCoordinates World coordinates of the block
     * \return True if such a block exists, false otherwise
     */
    [[nodiscard]] virtual bool doesWorldBlockExist(
        const Block::Coordinate& worldBlockCoordinates) const = 0;

    /**
     * \brief Converts a block in the indicated position into an air block
     * \param worldBlockCoordinates World coordinates of the block to change
     */
    virtual void removeWorldBlock(const Block::Coordinate& worldBlockCoordinates) = 0;

    /**
     * @brief Erases the chunk with the indicated coordinates
     * @param chunkCoordinate Coordinate the chunk to erase.
     * @return
     */
    virtual std::size_t erase(const ChunkContainerBase::Coordinate& chunkCoordinate) = 0;

    /**
     * @brief Returns whether a given chunk exists in the container
     * @param chunkPosition Position of the chunk sought
     * @return True if the object is in a container, false otherwise.
     */
    virtual bool isPresent(const ChunkContainerBase::Coordinate& chunkPosition) const = 0;

    /**
     * @brief It insert a block in a chunk with rebuilding the chunk. If there was another block
     * then it is overwritten.
     * @param id Block id
     * @param worldCoordinate World coordinates of the block to place
     */
    virtual void tryToPlaceBlock(const BlockId& id, Block::Coordinate worldCoordinate,
                                 std::vector<BlockId> blocksThatMightBeOverplaced) = 0;

    /**
     * @brief Returns information about whether the container is empty, that is, whether it does not
     * contain any chunks.
     * @return True if the container is empty, false otherwise.
     */
    virtual bool isEmpty() const = 0;

    /**
     * @brief Returns the number of chunks that are in the container.
     * @return Number of chunks that are in the container.
     */
    virtual std::size_t size() const = 0;
};
}// namespace Voxino