#pragma once

#include "World/Polygons/Block/Block.h"
#include "World/Polygons/Chunks/ChunkInterface.h"
#include "World/Polygons/Chunks/SimpleTerrainGenerator.h"
#include "World/Polygons/Meshes/Builders/ChunkMeshBuilder.h"
#include "World/Polygons/Meshes/Model3D.h"
#include <Resources/TexturePackArray.h>
#include <World/Polygons/Meshes/Builders/ChunkArrayMeshBuilder.h>
#include <memory>
#include <optional>
#include <vector>

namespace Voxino
{
class ChunkBlocks;
class SimpleTerrainGenerator;
class Renderer;
class Shader;
class ChunkContainer;

/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class Chunk : public ChunkInterface
{
public:
    Chunk(Block::Coordinate blockPosition, const TexturePackArray& texturePack,
          ChunkContainer& parent);
    Chunk(Block::Coordinate blockPosition, const TexturePackArray& texturePack);

    Chunk(Chunk&& rhs) noexcept;

    /**
     * Updates the status/logic of the state at equal intervals independent of the frame rate.
     * @param deltaTime Time interval
     */
    void fixedUpdate(const float& deltaTime) final;

    /**
     * Draws this chunk terrain to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawTerrain(const Renderer& renderer, const Shader& shader,
                     const Camera& camera) const final;

    /**
     * Draws this chunk liquids to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawLiquids(const Renderer& renderer, const Shader& shader,
                     const Camera& camera) const final;

    /**
     * Draws this chunk floral to the game screen
     * @param renderer Renderer drawing the 3D game world onto the 2D screen
     * @param shader Shader with the help of which the object should be drawn
     */
    void drawFlorals(const Renderer& renderer, const Shader& shader,
                     const Camera& camera) const final;

    /**
     * \brief Returns the block according to the coordinates given relative to the chunk position.
     * \param localCoordinates Position in relation to the chunk
     * \return Block reference inside chunk
     */
    Block& localBlock(const Block::Coordinate& localCoordinates) final;

    /**
     * \brief Removes a block on coordinates given relatively to the position of the chunk
     * \param localCoordinates Coordinates relative to the position of the chunk
     */
    void removeLocalBlock(const Block::Coordinate& localCoordinates) final;

    /**
     * \brief It tries to insert the indicated block in the indicated place in the chunk.
     * \param blockId Block Identifier
     * \param localCoordinates Coordinates relative to the position of the chunk
     * \param blocksThatMightBeOverplaced Blocks that can be overwritten by a function. Others
     * cannot be overwritten.
     */
    void tryToPlaceBlock(const BlockId& blockId, const Block::Coordinate& localCoordinates,
                         std::vector<BlockId> blocksThatMightBeOverplaced) final;

    /**
     * \brief Returns the block according to the coordinates given relative to the chunk position.
     * \param localCoordinates Position in relation to the chunk
     * \return Block reference inside chunk
     */
    [[nodiscard]] const Block& localBlock(const Block::Coordinate& localCoordinates) const final;

    /**
     * \brief Changes global world coordinates to local ones relative to chunk
     * \param worldCoordinates World coordinates of the block
     * \return Local coordinates of block relative to chunk
     */
    [[nodiscard]] Block::Coordinate globalToLocalCoordinates(
        const Block::Coordinate& worldCoordinates) const final;

    /**
     * \brief Changes local chunk coordinates to global ones inside the world
     * \param localCoordinates Local coordinates of the block
     * \return World coordinates of block inside the world
     */
    [[nodiscard]] Block::Coordinate localToGlobalCoordinates(
        const Block::Coordinate& localCoordinates) const final;

    /**
     * \brief Checks whether the local coordinates relative to the chunk are inside it
     * \param localCoordinates Local coordinates relative to chunk
     * \return True if local coordinates are inside the chunk, false otherwise
     */
    [[nodiscard]] static bool areLocalCoordinatesInsideChunk(
        const Block::Coordinate& localCoordinates);

    /**
     * \brief Checks that the given local coordinates relative to the chunk are at its extremity.
     * \param localCoordinates Local coordinates relative to chunk
     * \return True if the coordinates are on the edge of the chunk, false otherwise
     */
    [[nodiscard]] bool isLocalCoordinateOnChunkEdge(
        const Block::Coordinate& localCoordinates) final;

    /**
     * Returns information whether any chunk is in contact with the listed block. This is important
     * information when a given block has been destroyed and there is a need to rebuild the chunk
     * next to it so that it does not display an empty hole.
     * @param localCoordinates Local block coordinates of the block to be checked
     * @return Information on whether the block is in contact with another chunk
     */
    std::vector<Direction> directionOfBlockFacesInContactWithOtherChunk(
        const Block::Coordinate& localCoordinates) final;

    /**
     * Returns the position of the local block, located right next to the local block in the
     * indicated direction.
     * @param position Local block position
     * @param direction Direction next to which the block you are looking for is located
     * @return Block coordinate inside chunk
     */
    [[nodiscard]] Block::Coordinate localNearbyBlockPosition(
        const Block::Coordinate& position, const Direction& direction) const final;

    /**
     * Returns the block that is close to it, in the direction determined relative to the
     * block on the local coordinates.
     * @param position Local block coordinates inside the chunk
     * @param direction Direction next to which the block are looking for is located
     * @return Block reference inside chunk
     */
    [[nodiscard]] Block& localNearbyBlock(const Block::Coordinate& position,
                                          const Direction& direction) final;

    /**
     * Returns the block that is close to it, in the direction determined relative to the block on
     * the local coordinates.
     * @param position Local block coordinates inside the chunk
     * @param direction Direction next to which the block are looking for is located
     * @return Block reference inside chunk
     */
    [[nodiscard]] const Block& localNearbyBlock(const Block::Coordinate& localCoordinates,
                                                const Direction& direction) const final;

    /**
     * @brief Returns the position on the block scale.
     * @return Coordinates of the block of the beginning of the chunk.
     */
    const Block::Coordinate& positionInBlocks() const final;

    /**
     * @brief Finds a neighboring block located in the indicated direction
     * @param blockPos Position of the block for which the neighbor is sought
     * @param direction Direction from the block for which the neighbor is sought
     * @return Block if it exists, or nullopt if no such block exists.
     */
    std::optional<Block> neighbourBlockInGivenDirection(const Block::Coordinate& blockPos,
                                                        const Direction& direction) final;

protected:
    /**
     * Generates natural world terrain on a given chunk
     */
    void generateChunkTerrain();


    /**
     * It checks whether a given block face has an "air" or other transparent face next to it
     * through which it can be seen at all.
     *
     * @param blockFace The face of the block to check
     * @param blockPos Local position of the block to be checked
     * @return Yes if the face next to the face of the indicated block is transparent by which it is
     * visible, false in the real case
     */
    [[nodiscard]] bool doesBlockFaceHasTransparentNeighbor(const Block::Face& blockFace,
                                                           const Block::Coordinate& blockPos);


    /**
     * @brief Checks whether a given block face is in contact with a block of the specified type
     * @param blockFace Face of the block to be checked
     * @param blockPos Block position to be checked
     * @param blockId Block id
     * @return
     */
    [[nodiscard]] bool doesBlockFaceHasGivenBlockNeighbour(const Block::Face& blockFace,
                                                           const Block::Coordinate& blockPos,
                                                           const BlockId& blockId);

    /**
     * @brief Checks whether a block can be overwritten depending on the list of blocks that can be
     * overwritten and the id of the block that is trying to be overwritten.
     * @param blocksThatMightBeOverplaced List of blocks that can be overwritten
     * @param idOfTheBlockToOverplace The id of the block trying to overwrite
     * @return True if it can be overwritten, false otherwise.
     */
    bool canGivenBlockBeOverplaced(std::vector<BlockId>& blocksThatMightBeOverplaced,
                                   const BlockId& idOfTheBlockToOverplace) const;

    /**
     * @brief He is trying to put a block in the indicated local coordinates. A block is placed if
     * the overwritten block is included in the list of blocks that can be overwritten.
     * @param blockId The id of the block trying to insert in the given place.
     * @param localCoordinates Local coordinates of the chunk in place of which trying to insert a
     * block.
     * @param blocksThatMightBeOverplaced List of blocks that can be overwritten.
     */
    void tryToPlaceBlockInsideThisChunk(const BlockId& blockId,
                                        const Block::Coordinate& localCoordinates,
                                        std::vector<BlockId>& blocksThatMightBeOverplaced);


protected:
    std::unique_ptr<SimpleTerrainGenerator> mTerrainGenerator;
    Block::Coordinate mChunkPosition;
    const TexturePackArray& mTexturePack;
    ChunkContainer* mParentContainer;
    std::unique_ptr<ChunkBlocks> mChunkOfBlocks;

    std::unique_ptr<Model3D> mTerrainModel;
    std::unique_ptr<Model3D> mFluidModel;
    std::unique_ptr<Model3D> mFloralModel;
};
}// namespace Voxino