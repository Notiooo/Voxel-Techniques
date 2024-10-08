#pragma once

#include "BlockId.h"
#include "Utils/CoordinateBase.h"
#include "Utils/Direction.h"

#include <Utils/RGBA.h>


namespace Voxino
{

class BlockType;
class TexturePack;

class Block
{
public:
    Block();
    explicit Block(const BlockId& blockId);
    Block(const Block& rhs);

    /**
     * \brief The type of variable that is used to define the side of the block
     */
    using SizeType = int;

    /**
     * \brief Length of side in the space the block has
     */
    static constexpr SizeType BLOCK_SIZE = 1;

    /**
     * \brief Texture ID that corresponds to the texture by the position it is in the file.
     *
     * If you expand the texture file linearly and size from left to
     * right starting from zero, the position of the texture will
     * correspond to its id.
     *
     * An example:
     * Each '#' is an example texture
     *
     * #####
     * #####
     * #####
     *    ^
     *	This texture has an id of 14
     *
     */
    using TextureId = unsigned int;


    /**
     * \brief A spatial unit measured in blocks.
     *
     * For example when I want to get the fourth block from the bottom it would be {0, 4, 0}.
     *
     * It is useful to distinguish this unit from the usual spatial unit
     * where the block is at position {0, 4 * Block::BLOCK_SIZE, 0}.
     */
    struct Coordinate : public CoordinateBase
    {

        using CoordinateBase::CoordinateBase;

        /**
         * @brief Returns a neighboring coordinate in the indicated direction.
         * @param direction Direction from the block in the direction you want to get the
         * neighboring coordinate.
         * @return Neighboring coordinate in the indicated direction.
         */
        [[nodiscard]] Coordinate coordinateInGivenDirection(Direction direction) const;

        /**
         * \brief Converts the current position of a block to its positional equivalent in space.
         *
         * For example, if a block has dimension 16x16 and is at block
         * coordinates x: 0, y: 4, z: 0 then its position in space is:
         * { x: 0, y: 64, z: 0 }
         *
         * \return Positions in non-block-grid space
         */
        [[nodiscard]] sf::Vector3<Block::SizeType> nonBlockMetric() const;

        template<typename T>
        static Coordinate nonBlockToBlockMetric(const sf::Vector3<T>& nonBlockVector)
        {
            static auto fastFloor = [](const float& numberToFloor)
            {
                const auto truncated = static_cast<int>(numberToFloor);
                return truncated - (truncated > numberToFloor);
            };

            return Block::Coordinate(
                static_cast<SizeType>(fastFloor(nonBlockVector.x)) / BLOCK_SIZE,
                static_cast<SizeType>(fastFloor(nonBlockVector.y)) / BLOCK_SIZE,
                static_cast<SizeType>(fastFloor(nonBlockVector.z)) / BLOCK_SIZE);
        };
    };

    /**
     * Defines the face of the block.
     * Each block consists of 6 faces. Often there is no need to draw more faces, so this allows to
     * be more precise about which face referring to
     */
    enum class Face
    {
        Bottom = 0,
        Top,
        Left,
        Right,
        Front,
        Back,

        Counter
    };

    /**
     * @brief Returns the direction in which a particular block face is facing.
     * @param face The face of the block
     * @return The direction in which the given face of the block is facing.
     */
    [[nodiscard]] static Direction directionOfFace(Block::Face face);

    /**
     * Sets the block's settings to match those in the cfg files of the resources/blocks folder
     * @param blockId The id of the block
     */
    void setBlockType(const BlockId& blockId);

    /**
     * Retrieves the ID of the texture that is on the given block face
     * @param blockFace Selected block face from which texture will be read
     * @return ID of the corresponding texture face
     */
    [[nodiscard]] TextureId blockTextureId(const Block::Face& blockFace) const;

    /**
     * Returns a block id
     * @return Identifier of the block
     */
    [[nodiscard]] BlockId id() const;

    /**
     * Returns information about whether the block is transparent. For example, it can be glass.
     * @return True if the block is transparent, false otherwise
     */
    [[nodiscard]] bool isTransparent() const;

    /**
     * Returns information about whether the block is collidable. For example, it can be water/air.
     * @return True if the block is collidable, false otherwise
     */
    [[nodiscard]] bool isCollidable() const;

    /**
     * @brief Returns information whether the object is a plant part of the environment, such as
     * leaves or flowers. Such things are drawn separately so that under their transparency the area
     * behind them should be visible.
     */
    [[nodiscard]] bool isFloral() const;

    /**
     * @brief Converts the block to a RGBA color
     * @return RGBA color
     */
    [[nodiscard]] RGBA toRGBA() const;

    bool operator==(const Block& other) const
    {
        return id() == other.id();
    }


private:
    [[nodiscard]] std::array<GLubyte, 4> toBlockInShader() const;
    static float encodeTextureID(int highPart, int lowPart);


private:
    const BlockType* mBlockType;
};

}// namespace Voxino

namespace std
{

template<>
struct hash<Voxino::Block>
{
    std::size_t operator()(const Voxino::Block& k) const noexcept
    {
        return std::hash<Voxino::BlockId>()(k.id());
    }
};

}// namespace std