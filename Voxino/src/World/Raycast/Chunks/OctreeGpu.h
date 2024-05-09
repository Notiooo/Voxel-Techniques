#pragma once

#include "Renderer/Renderer.h"
#include "World/Block/Block.h"
#include "World/Chunks/ChunkBlocks.h"
#include <Renderer/Core/Buffers/AtomicCounter.h>
#include <array>
#include <cstdint>
#include <ranges>
#include <vector>

namespace Voxino::Raycast
{


class OctreeGpuNode
{
public:
    static constexpr int NO_CHILD = -1;
    static constexpr size_t NUMBER_OF_CHILDREN = 8;

private:
    // RGBA value;
    glm::uvec4 value;
    std::array<int, NUMBER_OF_CHILDREN> mChildren;

public:
    OctreeGpuNode()
        : value({0, 0, 0, 0})
        , mChildren{-1, -1, -1, -1, -1, -1, -1, -1}
    {
    }

    [[nodiscard]] std::array<int32_t, NUMBER_OF_CHILDREN> children() const
    {
        return mChildren;
    }

    // RGBA rgba() const
    // {
    //     return value;
    // }

    glm::uvec4 rgba() const
    {
        return value;
    }

    [[nodiscard]] bool hasChildren() const
    {
        return not std::ranges::all_of(mChildren,
                                       [](auto& child)
                                       {
                                           return child == -1;
                                       });
    }

    void block(const Block& block)
    {
        if (block.id() == BlockId::Air)
        {
            value = {0, 0, 0, 0};
        }
        else
        {
            auto rgba = block.toRGBA();
            value = {rgba.r, rgba.g, rgba.b, rgba.a};
        }
    }

    void child(size_t index, int32_t childId)
    {
        if (index < NUMBER_OF_CHILDREN) [[likely]]
        {
            mChildren[index] = childId;
        }
    }

    void setNoChildren()
    {
        mChildren.fill(NO_CHILD);
    }
};

constexpr int const_log2(int n)
{
    return (n <= 1) ? 0 : 1 + const_log2(n / 2);
}

struct OctreeNode
{
    std::array<int, 8> children;
    // RGBA data;
    glm::uvec4 data;
    // GLubyte padding[12];// 12 bytes padding to align the structure to 16 bytes
};

class OctreeGpu
{
public:
    constexpr static auto MAX_DEPTH =
        static_cast<int>(const_log2(ChunkBlocks::BLOCKS_PER_DIMENSION));
    OctreeGpu();
    ~OctreeGpu();

    void fillData(ChunkBlocks& chunk);

    std::vector<OctreeNode> serializeOctree();
    void draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const;

    /**
     * \brief Returns the number of ray iterations in the last frame.
     * \return The number of ray iterations in the last frame.
     */
    int lastNumberOfRayIterations() const;

    /**
     * \brief Updates the atomic counter and resets it.
     */
    void updateCounters();

    auto allocatedBytes() const
    {
        return mAllocatedBytes;
    }


private:
    std::vector<OctreeGpuNode> nodes;
    GLuint bufferID, texBufferID;
    struct Statistics
    {
        bool areAllBlocksTheSame;
        Block mostCommonBlock;
    };
    bool mNeedsBufferUpdate{false};
    int mLastNumberOfRayIterations{};
    AtomicCounter mAtomicCounter;
    bool mIsDataFilled{false};
    mutable bool mWasDataFiledLogPrinted{false};
    unsigned long long mAllocatedBytes{0};


private:
    [[nodiscard]] static Statistics gatherStatistics(const ChunkBlocks& chunk,
                                                     const glm::ivec3& position, int size);

    void buildOctree(const ChunkBlocks& chunk, const glm::ivec3& position, int size, int nodeIndex);

    GLuint uploadDataToOpenGL(const std::vector<OctreeNode>& data)
    {
        // glBindBuffer(GL_TEXTURE_BUFFER, bufferID);
        // glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(data[0]), data.data(),
        // GL_STATIC_DRAW); glBindTexture(GL_TEXTURE_BUFFER, texBufferID);
        // glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, bufferID);
        // glBindTexture(GL_TEXTURE_BUFFER, 0);
        // glBindBuffer(GL_TEXTURE_BUFFER, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(OctreeNode), data.data(),
                     GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        return bufferID;
    }
};

}// namespace Voxino::Raycast
