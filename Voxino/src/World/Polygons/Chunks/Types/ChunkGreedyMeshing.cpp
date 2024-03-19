#include "ChunkGreedyMeshing.h"
#include "pch.h"

#include "Utils/Bitset3D.h"
#include <Resources/TexturePack.h>

namespace Voxino
{


ChunkGreedyMeshing::ChunkGreedyMeshing(const Block::Coordinate& blockPosition,
                                       const TexturePackArray& texturePack, ChunkContainer& parent)
    : Chunk(blockPosition, texturePack, parent)
{
    initializeChunk();
}

ChunkGreedyMeshing::ChunkGreedyMeshing(const Block::Coordinate& blockPosition,
                                       const TexturePackArray& texturePack)
    : Chunk(blockPosition, texturePack)
{
    initializeChunk();
}

void ChunkGreedyMeshing::initializeChunk()
{
    prepareMesh();
    updateMesh();
}

void ChunkGreedyMeshing::prepareMesh()
{
    MEASURE_SCOPE;
    for (auto i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
    {
        ProcessedBlocks processedFaces;
        auto blockFace = static_cast<Block::Face>(i);
        const auto scanDirections = getScanDirectionsForFace(blockFace);
        for (const auto& [position, block]: *mChunkOfBlocks)
        {
            if (processedFaces.test(position))
            {
                continue;
            }

            const auto isAir = block.id() == BlockId::Air;
            const auto anyNeighbourTransparent =
                doesBlockFaceHasTransparentNeighbor(blockFace, position);
            if (isAir or not anyNeighbourTransparent)
            {
                processedFaces.set(position, true);
                continue;
            }

            createBlockMesh(
                tryMergeBiggestRegion(position, processedFaces, scanDirections, blockFace, block));
            processedFaces.set(position, true);
        }
    }
}

void ChunkGreedyMeshing::createBlockMesh(MeshRegion&& meshRegion)
{
    mTerrainMeshBuilder.addQuad(meshRegion);
}


MeshRegion ChunkGreedyMeshing::tryMergeBiggestRegion(const Block::Coordinate& pos,
                                                     ProcessedBlocks& processedFaces,
                                                     const ScanDirections& scanDirections,
                                                     const Block::Face& face, const Block& block)
{
    MeshRegion region;
    region.face = face;
    region.blockPosition = pos;
    region.width = 1;
    region.height = 1;
    region.id = block.blockTextureId(face);

    expandRegionHorizontally(region, processedFaces, scanDirections, block.blockTextureId(face),
                             face);
    expandRegionVertically(region, processedFaces, scanDirections, block.blockTextureId(face),
                           face);

    region.textureCoordinates = std::vector{
        glm::vec2(region.width, region.height),//
        glm::vec2(0, region.height),           //
        glm::vec2(0, 0),                       //
        glm::vec2(region.width, 0)             //
    };

    return region;
}

void ChunkGreedyMeshing::expandRegionHorizontally(MeshRegion& region,
                                                  ProcessedBlocks& processedFaces,
                                                  const ScanDirections& scanDirections,
                                                  Block::TextureId id, const Block::Face& face)
{
    for (auto nextBlockPosition = getNextPosition(region.blockPosition, scanDirections.first);
         canMerge(processedFaces, nextBlockPosition, id, face);
         nextBlockPosition = getNextPosition(nextBlockPosition, scanDirections.first))
    {
        ++region.width;
        processedFaces.set(nextBlockPosition, true);
    }
}

void ChunkGreedyMeshing::expandRegionVertically(MeshRegion& region, ProcessedBlocks& processedFaces,
                                                const ScanDirections& scanDirections,
                                                Block::TextureId id, const Block::Face& face)
{
    for (auto nextBlockPositionVertical =
             getNextPosition(region.blockPosition, scanDirections.second);
         canMerge(processedFaces, nextBlockPositionVertical, id, face);
         nextBlockPositionVertical =
             getNextPosition(nextBlockPositionVertical, scanDirections.second))
    {
        for (auto x = 0; x < region.width; ++x)
        {
            auto nextPosition = getNextPosition(nextBlockPositionVertical,
                                                static_cast<glm::ivec3>(scanDirections.first) * x);
            if (not canMerge(processedFaces, nextPosition, id, face))
            {
                return;
            }
        }
        for (auto x = 0; x < region.width; ++x)
        {
            auto nextPosition = getNextPosition(nextBlockPositionVertical,
                                                static_cast<glm::ivec3>(scanDirections.first) * x);
            processedFaces.set(nextPosition, true);
        }
        ++region.height;
    }
}

Block::Coordinate ChunkGreedyMeshing::getNextPosition(const Block::Coordinate& currentPosition,
                                                      const glm::ivec3& direction)
{
    return Block::Coordinate{currentPosition.x + direction.x, currentPosition.y + direction.y,
                             currentPosition.z + direction.z};
}

bool ChunkGreedyMeshing::canMerge(ProcessedBlocks& processedFaces,
                                  const Block::Coordinate& position, Block::TextureId id,
                                  const Block::Face& face)
{
    if (position.x >= ChunkBlocks::BLOCKS_PER_X_DIMENSION or
        position.y >= ChunkBlocks::BLOCKS_PER_Y_DIMENSION or
        position.z >= ChunkBlocks::BLOCKS_PER_Z_DIMENSION)
    {
        return false;
    }

    const auto wasProcessedBefore = processedFaces.test(position);
    const auto isAir = mChunkOfBlocks->block(position).id() == BlockId::Air;

    if (wasProcessedBefore or isAir)
    {
        return false;
    }

    const auto isDifferentTexture = mChunkOfBlocks->block(position).blockTextureId(face) != id;
    const auto isAnyNeighbourTransparent = doesBlockFaceHasTransparentNeighbor(face, position);

    if (isDifferentTexture or not isAnyNeighbourTransparent)
    {
        return false;
    }

    return true;
}

ChunkGreedyMeshing::ScanDirections ChunkGreedyMeshing::getScanDirectionsForFace(Block::Face face)
{
    switch (face)
    {
        case Block::Face::Top:
        case Block::Face::Bottom: return {{1, 0, 0}, {0, 0, 1}};// scan X and Z
        case Block::Face::Left:
        case Block::Face::Right: return {{0, 0, 1}, {0, 1, 0}};// scan Z and Y
        case Block::Face::Front:
        case Block::Face::Back: return {{1, 0, 0}, {0, 1, 0}};// scan X and Y
    }
}

}// namespace Voxino