#pragma once
#include "Renderer/Renderer.h"
#include "Resources/TexturePackArray.h"
#include "Utils/CoordinatesGenerator.h"
#include "World/Camera.h"
#include "World/Chunks/ChunkContainer.h"

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
class ChunkContainerPolygons : public ChunkContainer<ChunkType>
{
public:
    using Chunks = std::unordered_map<ChunkContainerBase::Coordinate, std::shared_ptr<ChunkType>,
                                      std::hash<CoordinateBase>>;

    ChunkContainerPolygons(const TexturePackArray& texturePackArray,
                           int radius = ChunkContainerBase::CHUNK_RADIUS)
        : ChunkContainer<ChunkType>(texturePackArray)
    {
        MEASURE_SCOPE;
        auto center = glm::vec3(0, 0, 0);
        auto coordinates = generateLimitedCoordinatesAround3D(center, radius);
        coordinates.push_back(center);
        for (auto coordinate: coordinates)
        {
            auto chunkPosition =
                Block::Coordinate(coordinate.x * ChunkBlocks::BLOCKS_PER_X_DIMENSION,
                                  coordinate.y * ChunkBlocks::BLOCKS_PER_Y_DIMENSION,
                                  coordinate.z * ChunkBlocks::BLOCKS_PER_Z_DIMENSION);
            auto newChunk = std::make_shared<ChunkType>(chunkPosition, texturePackArray, *this);
            auto chunkCoordinates =
                ChunkContainerBase::Coordinate::blockToChunkMetric(chunkPosition);
            this->emplace(chunkCoordinates, std::move(newChunk));
            this->rebuildChunksAround(chunkCoordinates);
        }
    }

    /**
     * @brief Rebuilds chunks around a given chunk.
     * @param chunkCoordinates Coordinates chunk around which other chunks should be rebuilt.
     */
    void rebuildChunksAround(ChunkContainerBase::Coordinate chunkCoordinates);

    int numberOfVertices() const
    {
        int vertices = 0;
        for (const auto& chunk: this->data())
        {
            vertices += chunk.second->numberOfVertices();
        }
        return vertices;
    }

    unsigned long memorySize() const
    {
        int size = 0;
        for (const auto& [_, chunk]: this->data())
        {
            size += chunk->memorySize();
        }
        return size;
    }

    // void draw(const Renderer& renderer, const Shader& shader,
    //                                  const Camera& camera) const
    // {
    //     // MEASURE_SCOPE_WITH_GPU;
    //     // for (auto& [coordinate, chunk]: this->data())
    //     // {
    //     //     chunk->draw(renderer, shader, camera);
    //     // }
    //
    //     // TODO: I could try to batch all draws here into one draw call
    // }
};


template<typename ChunkType>
void ChunkContainerPolygons<ChunkType>::rebuildChunksAround(
    ChunkContainerBase::Coordinate chunkCoordinates)
{
    auto& chunk = this->data().at(chunkCoordinates);
    for (auto direction: {Direction::Behind, Direction::InFront, Direction::ToTheLeft,
                          Direction::ToTheRight, Direction::Above, Direction::Below})
    {
        if (const auto chunkClose = this->chunkNearby(*chunk, direction))
        {
            chunkClose->rebuildMesh();
            chunkClose->updateMesh();
        }
    }
}

}// namespace Voxino