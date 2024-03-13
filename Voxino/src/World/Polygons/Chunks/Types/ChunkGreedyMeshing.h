#pragma once

#include "Utils/Bitset3D.h"
#include "World/Polygons/Chunks/Types/Chunk.h"
#include "World/Polygons/Meshes/MeshRegion.h"

#include <World/Polygons/Meshes/Builders/ChunkArrayMeshBuilder.h>

namespace Voxino
{

/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
class ChunkGreedyMeshing : public Chunk
{
public:
    ChunkGreedyMeshing(const Block::Coordinate& blockPosition, const TexturePack& texturePack,
                       ChunkContainer& parent);
    ChunkGreedyMeshing(const Block::Coordinate& blockPosition, const TexturePack& texturePack);

    /**
     * \brief Prepares/generates the mesh chunk, but does not replace it yet.
     */
    void prepareMesh() final;

    /**
     * \brief Swaps the current chunk mesh with the latest, most recently generated one
     */
    void updateMesh() final;

    /**
     * It is rebuilding this mesh fresh. Very expensive operation
     */
    void rebuildMesh() final;

private:
    using ScanDirections = std::pair<glm::bvec3, glm::bvec3>;
    using ProcessedBlocks =
        Bitset3D<ChunkBlocks::BLOCKS_PER_X_DIMENSION, ChunkBlocks::BLOCKS_PER_Y_DIMENSION,
                 ChunkBlocks::BLOCKS_PER_Z_DIMENSION>;

    /**
     * Initializes the chunk by preparing and updating its mesh.
     */
    void initializeChunk();

    /**
     * Calculates the next position based on the current position and a direction vector.
     * This function is used to determine what next block should be processed.
     *
     * @param currentPosition The current position of the block.
     * @param direction A vector indicating the direction of movement.
     * @return The new position after applying the direction to the current position.
     */
    Block::Coordinate getNextPosition(const Block::Coordinate& currentPosition,
                                      const glm::ivec3& direction);

    /**
     * Attempts to merge adjacent faces into the largest possible mesh region for a given block
     * face.
     *
     * @param pos Position of the current block being processed.
     * @param processedFaces Tracks faces that have already been processed to avoid duplication.
     * @param scanDirections Directions to check for potential mergeable adjacent faces.
     * @param face The current face of the block being processed.
     * @param block The block instance being processed.
     * @return The largest possible mesh region created from merging adjacent faces.
     */
    MeshRegion tryMergeBiggestRegion(const Block::Coordinate& pos, ProcessedBlocks& processedFaces,
                                     const ScanDirections& scanDirections, const Block::Face& face,
                                     const Block& block);

    /**
     * Determines valid scan directions based on the face orientation for merging.
     *
     * @param face The face for which scan directions are needed.
     * @return Scan directions suitable for the given face to facilitate mesh merging.
     */
    ScanDirections getScanDirectionsForFace(Block::Face face);

    /**
     * Checks if the block at a given position with a specific texture ID can be merged
     *
     * @param processedFaces Keeps track of faces that have been considered for merging.
     * @param position Position of the block to check for merging capability.
     * @param id Texture ID of the block, used to ensure consistency in merging.
     * @param face The face of the block being considered for merging.
     * @return True if the block can be merged, false otherwise.
     */
    bool canMerge(ProcessedBlocks& processedFaces, const Block::Coordinate& position,
                  Block::TextureId id, const Block::Face& face);

    /**
     * Expands a mesh region horizontally based on the available adjacent faces that match criteria.
     *
     * @param region The current mesh region being expanded.
     * @param processedFaces Tracks faces already processed or included in a region.
     * @param scanDirections Directions to scan for expansion.
     * @param id Texture ID required for expansion to ensure visual consistency.
     * @param face The face orientation guiding the expansion process.
     */
    void expandRegionHorizontally(MeshRegion& region, ProcessedBlocks& processedFaces,
                                  const ScanDirections& scanDirections, Block::TextureId id,
                                  const Block::Face& face);

    /**
     * Expands a mesh region vertically, similar to horizontal expansion
     *
     * @param region Mesh region to expand.
     * @param processedFaces Tracks already considered faces.
     * @param scanDirections Directions to look for possible expansion.
     * @param id Texture ID to maintain consistency in the mesh.
     * @param face The block face that determines expansion direction.
     */
    void expandRegionVertically(MeshRegion& region, ProcessedBlocks& processedFaces,
                                const ScanDirections& scanDirections, Block::TextureId id,
                                const Block::Face& face);

    /**
     * Generates the block mesh for a given mesh region. This finalizes the mesh creation process.
     *
     * @param meshRegion The mesh region for which a block mesh is to be created.
     */
    void createBlockMesh(MeshRegion&& meshRegion);

private:
    // TODO: This system should be changed to a better one. Consider distance.
    ChunkArrayMeshBuilder mTerrainMeshBuilder;
    ChunkArrayMeshBuilder mFluidMeshBuilder;
    ChunkArrayMeshBuilder mFloralMeshBuilder;
};
}// namespace Voxino