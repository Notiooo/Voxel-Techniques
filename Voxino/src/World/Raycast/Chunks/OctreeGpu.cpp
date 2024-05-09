#include "OctreeGpu.h"
#include "pch.h"

Voxino::Raycast::OctreeGpu::OctreeGpu()
{
    nodes.emplace_back();// Start with root node
    // glGenBuffers(1, &bufferID);
    // glGenTextures(1, &texBufferID);
    // glBindTexture(GL_TEXTURE_BUFFER, 0);// Unbind the texture
    // glBindBuffer(GL_TEXTURE_BUFFER, 0); // Unbind the buffer

    glGenBuffers(1, &bufferID);
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
Voxino::Raycast::OctreeGpu::~OctreeGpu()
{
    glDeleteBuffers(1, &bufferID);
}

void Voxino::Raycast::OctreeGpu::draw(const Renderer& renderer, const Shader& shader,
                                      const Camera& camera) const
{
    shader.bind();
    mAtomicCounter.bind();
    // GLCall(glActiveTexture(GL_TEXTURE1));
    // glBindTexture(GL_TEXTURE_3D, texBufferID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufferID);
    shader.setUniform("u_OctreeData", 1);
    shader.setUniform("u_WorldSize", glm::ivec3(ChunkBlocks::BLOCKS_PER_DIMENSION,
                                                ChunkBlocks::BLOCKS_PER_DIMENSION,
                                                ChunkBlocks::BLOCKS_PER_DIMENSION));
    renderer.drawRaycast(shader, camera);
}

int Voxino::Raycast::OctreeGpu::lastNumberOfRayIterations() const
{
    return mLastNumberOfRayIterations;
}

void Voxino::Raycast::OctreeGpu::updateCounters()
{
    mLastNumberOfRayIterations = mAtomicCounter.read();
    mAtomicCounter.reset();
}

void Voxino::Raycast::OctreeGpu::fillData(ChunkBlocks& chunk)
{
    constexpr auto startingPosition = glm::ivec3(0, 0, 0);
    constexpr auto startingNode = 0;
    buildOctree(chunk, startingPosition, ChunkBlocks::BLOCKS_PER_DIMENSION, startingNode);
    auto serializedData = serializeOctree();
    mAllocatedBytes = serializedData.size() * sizeof(OctreeNode);
    uploadDataToOpenGL(serializedData);
}

std::vector<Voxino::Raycast::OctreeNode> Voxino::Raycast::OctreeGpu::serializeOctree()
{
    std::vector<OctreeNode> flatData;

    for (int oldIndex = 0; oldIndex < nodes.size(); ++oldIndex)
    {
        const auto& node = nodes[oldIndex];

        // Serialize RGBA value as a single integer
        OctreeNode flatOctreeNode;
        flatOctreeNode.children = node.children();
        flatOctreeNode.data = node.rgba();
        flatData.push_back(flatOctreeNode);
    }
    return flatData;
}

Voxino::Raycast::OctreeGpu::Statistics Voxino::Raycast::OctreeGpu::gatherStatistics(
    const ChunkBlocks& chunk, const glm::ivec3& position, int size)
{
    std::unordered_map<Block, int> blockCount;
    const auto& firstBlock = chunk.block(position.x, position.y, position.z);
    auto firstBlockId = firstBlock.id();
    bool areAllBlocksSame = true;

    for (int x = 0; x < size; ++x)
    {
        for (int y = 0; y < size; ++y)
        {
            for (int z = 0; z < size; ++z)
            {
                const auto& currentBlock =
                    chunk.block(position.x + x, position.y + y, position.z + z);
                blockCount[currentBlock]++;
                if (currentBlock.id() != firstBlockId)
                {
                    areAllBlocksSame = false;
                }
            }
        }
    }

    const auto& [block, count] = *std::ranges::max_element(std::as_const(blockCount),
                                                           [](const auto& a, const auto& b)
                                                           {
                                                               return a.second < b.second;
                                                           });

    return {areAllBlocksSame, block};
}

void Voxino::Raycast::OctreeGpu::buildOctree(const ChunkBlocks& chunk, const glm::ivec3& position,
                                             int size, int nodeIndex)
{
    auto stats = gatherStatistics(chunk, position, size);
    if (stats.areAllBlocksTheSame)
    {
        nodes[nodeIndex].block(stats.mostCommonBlock);
        if (stats.mostCommonBlock.id() == BlockId::Air)
        {
            nodes[nodeIndex].setNoChildren();
        }
        return;
    }

    if (size == 1)
    {
        return;// Base case
    }

    int half = size / 2;
    for (int dx = 0; dx < size; dx += half)
    {
        for (int dy = 0; dy < size; dy += half)
        {
            for (int dz = 0; dz < size; dz += half)
            {
                glm::ivec3 childPos = position + glm::ivec3(dx, dy, dz);
                const auto childIndex = nodes.size();
                const auto index = (dx / half) + (dy / half) * 2 + (dz / half) * 4;
                nodes[nodeIndex].child(index, childIndex);
                nodes.emplace_back();
                buildOctree(chunk, childPos, half, childIndex);
            }
        }
    }
}