#pragma once

#include "Utils/RGBA.h"
#include "World/Chunks/Chunk.h"
#include "World/Raycast/Chunks/VoxelsGpu.h"

#include <deque>

namespace Voxino::Raycast
{

/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class RaycastChunk : public Chunk
{
public:
    RaycastChunk(const Block::Coordinate& blockPosition, const TexturePackArray& texturePack);

    /**
     * Returns the number of chunk vertices
     * @return Number of vertices
     */
    int numberOfVertices() override;

    /**
     * Returns the size in memory that the mesh occupies
     * @return The size in memory in bytes that the chunk occupies
     */
    int memorySize() override;

    /**
     * \brief Removes a block on coordinates given relatively to the position of the chunk
     * \param localCoordinates Coordinates relative to the position of the chunk
     */
    void removeLocalBlock(const Block::Coordinate& localCoordinates) override;

    /**
     * Draws this chunk to the game screen
     * \param renderer Renderer drawing the 3D game world onto the 2D screen
     * \param shader Shader with the help of which the object should be drawn
     * \param camera A camera in 3D space that looks at this object
     */
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const override;

    /**
     * \brief Updates the state logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     */
    void update(const float& deltaTime) override;

    /**
     * \brief Updates the status/logic of the ImGui Debug Menu
     */
    void updateImGui() override;

protected:
    /**
     * \brief It tries to insert the indicated block in the indicated place in the chunk.
     * \param blockId Block Identifier
     * \param localCoordinates Coordinates relative to the position of the chunk
     * \param blocksThatMightBeOverplaced Blocks that can be overwritten by a function. Others
     * cannot be overwritten.
     */
    bool tryToPlaceBlockInsideThisChunk(const BlockId& blockId,
                                        const Block::Coordinate& localCoordinates,
                                        std::vector<BlockId>& blocksThatMightBeOverplaced) override;

private:
    std::array<GLubyte, 4> toBlockInShader(const Block& block);
    float encodeTextureID(int highPart, int lowPart);
    RGBA toRGBA(const Voxino::Block& block);
    void fillData();

private:
    VoxelsGpu mVoxels;
};

}// namespace Voxino::Raycast