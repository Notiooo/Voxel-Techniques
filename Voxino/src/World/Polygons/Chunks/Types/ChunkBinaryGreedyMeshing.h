#pragma once

#include "World/Chunks/ChunkBlocks.h"
#include "World/Polygons/Chunks/Types/ChunkArray.h"
#include "World/Polygons/Meshes/Builders/ChunkArrayMeshBuilder.h"
#include "World/Polygons/Meshes/MeshRegion.h"

namespace Voxino::Polygons
{

/**
 * \brief A chunk that uses the greedy meshing algorithm.
 */
class ChunkBinaryGreedyMeshing : public ChunkArray<ChunkArrayMeshBuilder>
{
public:
    static constexpr int PLANE_SIZE = ChunkBlocks::BLOCKS_PER_DIMENSION;
    static constexpr int PLANE_SIZE_P = PLANE_SIZE + 2;
    static constexpr int PLANE_SIZE_P2 = PLANE_SIZE_P * PLANE_SIZE_P;
    static constexpr int PLANE_SIZE_P3 = PLANE_SIZE_P * PLANE_SIZE_P * PLANE_SIZE_P;
    using BinaryRow = std::array<uint32_t, PLANE_SIZE>;
    using BinaryPlane = std::unordered_map<uint32_t, BinaryRow>;
    using PlaneMap = std::unordered_map<uint32_t, BinaryPlane>;
    using BlockDataMap = std::array<PlaneMap, static_cast<int>(Block::Face::Counter)>;
    using FaceCullingMask = std::array<uint64_t, 3 * PLANE_SIZE_P2 * 2>;
    using AxisEncodedBitSequences = std::array<uint64_t, 3 * PLANE_SIZE_P2>;


    ChunkBinaryGreedyMeshing(const Block::Coordinate& blockPosition,
                             const TexturePackArray& texturePack);

    /**
     * \brief Prepares/generates the mesh chunk, but does not replace it yet.
     */
    void prepareMesh() final;

private:
    /** \brief Represents a quad in the greedy meshing algorithm. */
    struct GreedyQuad
    {
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
    };

    /**
     * Initializes the chunk by preparing and updating its mesh.
     */
    void initializeChunk();

    /**
     * Builds culling masks for each face of blocks to optimize rendering.
     * @return Fully constructed culling masks.
     */
    [[nodiscard]] FaceCullingMask buildFaceCullingMasks() const;

    /**
     * Constructs binary planes to represent block data in terms of visibility and structure.
     * @return A map of binary plane data organized by block face.
     */
    [[nodiscard]] BlockDataMap buildBinaryPlanes() const;

    /**
     * Executes a greedy meshing algorithm on a single plane to reduce the number of draw calls.
     * @param data The binary data representing a single plane.
     * @param lod_size Level of detail size, affecting the granularity of meshing.
     * @return A collection of meshed quads.
     */
    std::vector<GreedyQuad> greedyMeshSinglePlane(std::array<uint32_t, PLANE_SIZE>& data,
                                                  uint32_t lod_size);

    /**
     * Calculates the 3D voxel position based on block face and 2D coordinates.
     * @param blockFace The face of the block determining the axes.
     * @param x, y, z Coordinate components.
     * @return The computed 3D voxel position.
     */
    static glm::ivec3 calculateVoxelPosition(Block::Face blockFace, int x, int y, int z);

    /**
     * Creates a bitmask with all bits set to 1 up to a specified bit position.
     * @param shift Number of least significant bits set to 1.
     * @return A bitmask with specified bits set.
     */
    static uint32_t generateAllOnesMask(unsigned int shift);

    /**
     * Constructs encoded bit sequences for axes, facilitating efficient space and visibility
     * checks.
     * @return Encoded bit sequences for all three axes.
     */
    [[nodiscard]] AxisEncodedBitSequences generateAxisEncodedBitSequences() const;

    /**
     * Creates mesh regions for a specific axis by analyzing the binary planes.
     * @param binaryPlanes The binary plane data.
     * @param axis Index of the axis for which the mesh is generated.
     * @return A list of mesh regions for efficient rendering.
     */
    std::vector<MeshRegion> generateMeshForAxis(BlockDataMap& binaryPlanes, int axis);

    /**
     * Extracts rectangular regions from a binary plane that can be meshed together.
     * @param blockFace The face of the block being processed.
     * @param blockType Type identifier for the block.
     * @param axisPlane The specific plane data for the given block type and axis.
     * @return Meshable regions derived from the plane.
     */
    std::vector<MeshRegion> extractRegionsFromPlane(Block::Face blockFace, uint32_t blockType,
                                                    BinaryPlane& axisPlane);

    /**
     * Constructs a mesh region from a given quad.
     * @param blockFace The face of the block.
     * @param blockType The type of block.
     * @param axisPos Position along the axis being processed.
     * @param quad The quad to be converted into a mesh region.
     * @return The constructed mesh region.
     */
    static MeshRegion createMeshRegion(Block::Face blockFace, uint32_t blockType, int axisPos,
                                       const GreedyQuad& quad);

    /**
     * Computes block coordinates from a quad's dimensions and position.
     * @param blockFace The face of the block.
     * @param quad Details of the quad including dimensions and position.
     * @param axis_pos Position along the axis.
     * @return The computed block coordinates.
     */
    static Block::Coordinate computeBlockCoordinates(Block::Face blockFace, const GreedyQuad& quad,
                                                     int axis_pos);

    /**
     * Calculates texture coordinates for a quad, based on its dimensions.
     * @param quad The quad whose texture coordinates are being calculated.
     * @return A list of texture coordinates corresponding to the quad corners.
     */
    static std::vector<glm::vec2> calculateTextureCoordinates(const GreedyQuad& quad);

    /**
     * Processes binary row data to extract quads, expanding the efficient rendering of contiguous
     * areas.
     * @param data The binary row data.
     * @param quads Collection to store the generated quads.
     * @param row The current row being processed.
     * @param planeSize The size of the plane, defining boundary conditions.
     */
    static void processRowForQuads(BinaryRow& data, std::vector<GreedyQuad>& quads, size_t row,
                                   uint32_t planeSize);

    /**
     * Identifies the start of non-zero data in a row, optimizing mesh generation.
     * @param rowData The binary row data.
     * @param startIndex The starting index for the search.
     * @return The index of the first non-zero element.
     */
    static uint32_t skipLeadingZeros(uint32_t rowData, uint32_t startIndex);

    /**
     * Counts the number of consecutive set bits starting from a given index, aiding in quad
     * formation.
     * @param rowData The binary row data.
     * @param startIndex The starting index for counting.
     * @return The count of consecutive ones.
     */
    static uint32_t countConsecutiveOnes(uint32_t rowData, uint32_t startIndex);

    /**
     * Generates a shifted bitmask with a specified width of set bits, used in voxel processing.
     * @param width The width of the mask in bits.
     * @param offset The starting bit position for the mask.
     * @return The generated bitmask.
     */
    static uint32_t generateMask(uint32_t width, uint32_t offset);

    /**
     * Expands and clears horizontal segments of a row, optimizing space representation in memory.
     * @param data The binary row data.
     * @param startRow The starting row for expansion.
     * @param startColumn The starting column.
     * @param widthAsMask The width of the mask used for expansion.
     * @param mask The mask applied to clear bits.
     * @param planeSize The total size of the plane for boundary checks.
     * @return The number of rows successfully expanded and cleared.
     */
    static uint32_t expandAndClearRow(BinaryRow& data, size_t startRow, uint32_t startColumn,
                                      uint32_t widthAsMask, uint32_t mask, uint32_t planeSize);

    /**
     * Stores culling mask data for a block in a specified plane, aiding in efficient rendering.
     * @param data Reference to the data structure where masks are stored.
     * @param axis The axis index being processed.
     * @param blockFace The face of the block.
     * @param z The z-coordinate in the plane.
     * @param x The x-coordinate in the plane.
     * @param faceCullingMask Reference to the culling mask to be stored.
     */
    void storeFaceCullingMaskInPlaneForGivenBlock(BlockDataMap& data, int axis,
                                                  Block::Face blockFace, int z, int x,
                                                  unsigned long long& faceCullingMask) const;
};

}// namespace Voxino::Polygons