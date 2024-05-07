#include "ChunkBinaryGreedyMeshing.h"
#include "Utils/Bitset3D.h"
#include "pch.h"
#include <bit>

#include <Resources/TexturePack.h>

namespace Voxino::Polygons
{

ChunkBinaryGreedyMeshing::AxisEncodedBitSequences ChunkBinaryGreedyMeshing::
    generateAxisEncodedBitSequences() const
{
    /**
     * We construct a cube, where for each x and y we have a sequence of bits (uint64) corresponding
     * to the z axis. For each x and z, we have a sequence of bits corresponding to the y axis.
     * And so on
     **/

    AxisEncodedBitSequences axisEncodedBits{};

    for (auto x = 0; x < PLANE_SIZE_P; ++x)
    {
        for (auto y = 0; y < PLANE_SIZE_P; ++y)
        {
            for (auto z = 0; z < PLANE_SIZE_P; ++z)
            {
                auto localCoordinates = Block::Coordinate(x - 1, y - 1, z - 1);

                // TODO: MAKE USE OF PARENT CONTAINER
                if (areLocalCoordinatesInsideChunk(localCoordinates))
                {
                    auto block = mChunkOfBlocks->block(localCoordinates);
                    if (not block.isTransparent())
                    {
                        // For each x and z we get binary values  representing y axis of chunk
                        axisEncodedBits[x + (z * PLANE_SIZE_P)] |= 1ULL << y;

                        // For each z and y we get binary values representing x axis of chunk
                        axisEncodedBits[z + (y * PLANE_SIZE_P) + PLANE_SIZE_P2] |= 1ULL << x;

                        // For each x and y we get binary values representing z axis of chunk
                        axisEncodedBits[x + (y * PLANE_SIZE_P) + PLANE_SIZE_P2 * 2] |= 1ULL << z;
                    }
                }
                else
                {
                    // TODO: Get global block and check if it is transparent
                }
            }
        }
    }
    return axisEncodedBits;
}

ChunkBinaryGreedyMeshing::FaceCullingMask ChunkBinaryGreedyMeshing::buildFaceCullingMasks() const
{
    /**
     * Each axis contains a sequence of bits defining the transparency of the block. X and Y
     * contains the bit sequence representing the blocks on Z. X and Z represents the blocks on Y
     * and so on. As a result, we get a cube. Now for each such bit sequence, for example,
     * representing the z-axis on the basis of bit operations, we get information on which side of
     * the bit the face should be created (in this case, left or right). Therefore, we keep the
     * resulting bit sequence separately for the left face and separately for the right face.
     **/

    auto axisEncodedBits = generateAxisEncodedBitSequences();
    FaceCullingMask cullingMask{};

    for (auto axis = 0; axis < 3; ++axis)
    {
        for (auto i = 0; i < PLANE_SIZE_P2; ++i)
        {
            // set if current is solid, and next is air
            auto col = axisEncodedBits[PLANE_SIZE_P2 * axis + i];

            // sample ascending axis, and set true when air meets solid
            cullingMask[(PLANE_SIZE_P2 * (axis * 2 + 1)) + i] = col & ~(col >> 1);
            // sample descending axis, and set true when air meets solid
            cullingMask[(PLANE_SIZE_P2 * (axis * 2 + 0)) + i] = col & ~(col << 1);
        }
    }

    return cullingMask;
}

glm::ivec3 ChunkBinaryGreedyMeshing::calculateVoxelPosition(Block::Face blockFace, int x, int y,
                                                            int z)
{
    switch (blockFace)
    {
        case Block::Face::Bottom:
        case Block::Face::Top: return {x, y, z};

        case Block::Face::Left:
        case Block::Face::Right: return {y, z, x};

        case Block::Face::Front:
        case Block::Face::Back: return {x, z, y};
        default: break;
    }
    throw std::invalid_argument("Invalid axis value");
}

void ChunkBinaryGreedyMeshing::storeFaceCullingMaskInPlaneForGivenBlock(
    BlockDataMap& data, int axis, Block::Face blockFace, int z, int x,
    unsigned long long& faceCullingMask) const
{
    while (faceCullingMask != 0)
    {
        auto y = static_cast<unsigned long>(std::countr_zero(faceCullingMask));
        faceCullingMask &= faceCullingMask - 1;

        auto voxelPos = calculateVoxelPosition(blockFace, x, y, z);

        auto blockHash =
            static_cast<uint32_t>(mChunkOfBlocks->block(voxelPos).blockTextureId(blockFace));
        auto& plane = data[axis][blockHash][y];
        plane[x] |= (1u << z);
    }
}

ChunkBinaryGreedyMeshing::BlockDataMap ChunkBinaryGreedyMeshing::buildBinaryPlanes() const
{
    const auto faceCullingMasks = buildFaceCullingMasks();

    BlockDataMap data;
    for (auto axis = 0; axis < static_cast<int>(Block::Face::Counter); ++axis)
    {
        auto blockFace = static_cast<Block::Face>(axis);
        for (auto z = 0; z < PLANE_SIZE; ++z)
        {
            for (auto x = 0; x < PLANE_SIZE; ++x)
            {
                // skip padded by adding 1(for x padding) and (z+1) for (z padding)
                auto indexOfBitSequence = 1 + x + ((z + 1) * PLANE_SIZE_P) + PLANE_SIZE_P2 * axis;
                // removes the right most padding value, because it's invalid
                auto faceCullingMask = faceCullingMasks[indexOfBitSequence] >> 1;
                // removes the left most padding value, because it's invalid
                faceCullingMask &= ~(1ULL << PLANE_SIZE);


                storeFaceCullingMaskInPlaneForGivenBlock(data, axis, blockFace, z, x,
                                                         faceCullingMask);
            }
        }
    }
    return data;
}

uint32_t ChunkBinaryGreedyMeshing::generateAllOnesMask(unsigned int shift)
{
    constexpr uint32_t maxShift = 32;
    if (shift >= maxShift)
    {
        return ~0u;
    }
    uint32_t result = 1u << shift;
    return result - 1;
}

std::vector<ChunkBinaryGreedyMeshing::GreedyQuad> ChunkBinaryGreedyMeshing::greedyMeshSinglePlane(
    BinaryRow& data, uint32_t planeSize)
{
    std::vector<GreedyQuad> quads;
    size_t rowCount = data.size();

    for (size_t row = 0; row < rowCount; ++row)
    {
        processRowForQuads(data, quads, row, planeSize);
    }

    return quads;
}

void ChunkBinaryGreedyMeshing::processRowForQuads(BinaryRow& data, std::vector<GreedyQuad>& quads,
                                                  size_t row, uint32_t planeSize)
{
    uint32_t y = 0;
    while (y < planeSize)
    {
        y += skipLeadingZeros(data[row], y);
        if (y >= planeSize)
        {
            break;
        }

        uint32_t segmentWidth = countConsecutiveOnes(data[row], y);
        uint32_t widthAsMask = generateAllOnesMask(segmentWidth);
        uint32_t mask = widthAsMask << y;

        uint32_t horizontalGrowth = expandAndClearRow(data, row, y, widthAsMask, mask, planeSize);
        quads.push_back(GreedyQuad{static_cast<uint32_t>(row), y, horizontalGrowth, segmentWidth});

        y += segmentWidth;
    }
}

uint32_t ChunkBinaryGreedyMeshing::skipLeadingZeros(uint32_t rowData, uint32_t startIndex)
{
    return std::countr_zero(rowData >> startIndex);
}

uint32_t ChunkBinaryGreedyMeshing::countConsecutiveOnes(uint32_t rowData, uint32_t startIndex)
{
    return std::countr_one(rowData >> startIndex);
}

uint32_t ChunkBinaryGreedyMeshing::generateMask(uint32_t width, uint32_t offset)
{
    uint32_t mask = (1U << width) - 1;// Creates a mask of 'width' ones
    return mask << offset;
}

uint32_t ChunkBinaryGreedyMeshing::expandAndClearRow(BinaryRow& data, size_t startRow,
                                                     uint32_t startColumn, uint32_t widthAsMask,
                                                     uint32_t mask, uint32_t planeSize)
{
    uint32_t width = 1;
    while ((startRow + width) < planeSize)
    {
        uint32_t nextRowSegment = (data[startRow + width] >> startColumn) & widthAsMask;
        if (nextRowSegment != widthAsMask)
        {
            break;
        }

        data[startRow + width] &= ~mask;
        ++width;
    }
    return width;
}


ChunkBinaryGreedyMeshing::ChunkBinaryGreedyMeshing(const Block::Coordinate& blockPosition,
                                                   const TexturePackArray& texturePack)
    : ChunkArray(blockPosition, texturePack)
{
    initializeChunk();
}

void ChunkBinaryGreedyMeshing::initializeChunk()
{
    prepareMesh();
    updateMesh();
}

void ChunkBinaryGreedyMeshing::prepareMesh()
{
    MEASURE_SCOPE;
    auto binaryPlanes = buildBinaryPlanes();
    for (int axis = 0; axis < static_cast<int>(Block::Face::Counter); ++axis)
    {
        auto meshRegions = generateMeshForAxis(binaryPlanes, axis);
        for (const auto& meshRegion: meshRegions)
        {
            mTerrainMeshBuilder.addQuad(meshRegion);
        }
    }
}

std::vector<MeshRegion> ChunkBinaryGreedyMeshing::generateMeshForAxis(BlockDataMap& binaryPlanes,
                                                                      int axis)
{
    std::vector<MeshRegion> meshRegions;
    auto blockFace = static_cast<Block::Face>(axis);
    auto& planeData = binaryPlanes[axis];

    for (auto& [blockType, plane]: planeData)
    {
        auto regions = extractRegionsFromPlane(blockFace, blockType, plane);
        meshRegions.insert(meshRegions.end(), regions.begin(), regions.end());
    }
    return meshRegions;
}

std::vector<MeshRegion> ChunkBinaryGreedyMeshing::extractRegionsFromPlane(Block::Face blockFace,
                                                                          uint32_t blockType,
                                                                          BinaryPlane& plane)
{
    std::vector<MeshRegion> regions;
    for (auto& [position, singlePlane]: plane)
    {
        auto quads = greedyMeshSinglePlane(singlePlane, PLANE_SIZE);
        for (const auto& quad: quads)
        {
            regions.push_back(createMeshRegion(blockFace, blockType, position, quad));
        }
    }
    return regions;
}

MeshRegion ChunkBinaryGreedyMeshing::createMeshRegion(Block::Face blockFace, uint32_t blockType,
                                                      int position, const GreedyQuad& quad)
{
    MeshRegion region;
    region.face = blockFace;
    region.id = blockType;
    region.width = quad.width;
    region.height = quad.height;
    region.blockPosition = computeBlockCoordinates(blockFace, quad, position);
    region.textureCoordinates = calculateTextureCoordinates(quad);
    return region;
}

Block::Coordinate ChunkBinaryGreedyMeshing::computeBlockCoordinates(Block::Face blockFace,
                                                                    const GreedyQuad& quad,
                                                                    int position)
{
    switch (blockFace)
    {
        case Block::Face::Bottom:
        case Block::Face::Top: return Block::Coordinate(quad.x, position, quad.y);
        case Block::Face::Left:
        case Block::Face::Right: return Block::Coordinate(position, quad.y, quad.x);
        case Block::Face::Front: return Block::Coordinate(quad.x, quad.y, position - 1);
        case Block::Face::Back: return Block::Coordinate(quad.x, quad.y, position + 1);
        default: throw std::invalid_argument("Unsupported block face");
    }
}

std::vector<glm::vec2> ChunkBinaryGreedyMeshing::calculateTextureCoordinates(const GreedyQuad& quad)
{
    return {
        glm::vec2(quad.width, quad.height),//
        glm::vec2(0, quad.height),         //
        glm::vec2(0, 0),                   //
        glm::vec2(quad.width, 0)           //
    };
}

}// namespace Voxino::Polygons