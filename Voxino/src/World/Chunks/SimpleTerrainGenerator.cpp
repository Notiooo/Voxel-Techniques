#include "SimpleTerrainGenerator.h"
#include "Chunk.h"
#include "pch.h"

namespace Voxino
{
SimpleTerrainGenerator::SimpleTerrainGenerator(int seed)
    : mBasicTerrain(seed)
{
    mBasicTerrain.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    mBasicTerrain.SetFrequency(0.001);
    mBasicTerrain.SetFractalGain(0);
    mBasicTerrain.SetFractalLacunarity(0.f);
    mBasicTerrain.SetFractalOctaves(1);
    mBasicTerrain.SetSeed(seed);
}

void SimpleTerrainGenerator::generateTerrain(Chunk& chunk, ChunkBlocks& chunkBlocks)
{
    generateTerrainForChunk(chunk, chunkBlocks);
}


void SimpleTerrainGenerator::generateTerrainForChunk(const Chunk& chunk, ChunkBlocks& chunkBlocks)
{
    for (auto x = 0; x < ChunkBlocks::BLOCKS_PER_X_DIMENSION; ++x)
    {
        for (auto z = 0; z < ChunkBlocks::BLOCKS_PER_Z_DIMENSION; ++z)
        {
            auto globalCoord = chunk.localToGlobalCoordinates({x, 0, z});
            auto surfaceLevel = surfaceLevelAtGivenPosition(globalCoord.x, globalCoord.z);
            generateColumnOfBlocks(chunkBlocks, surfaceLevel, x, globalCoord.y, z);
        }
    }
}

int SimpleTerrainGenerator::surfaceLevelAtGivenPosition(int blockCoordinateX, int blockCoordinateZ)
{
    auto basicTerrainNoise = mBasicTerrain.GetNoise(static_cast<float>(blockCoordinateX),
                                                    static_cast<float>(blockCoordinateZ));

    auto heightOfBlocks = (basicTerrainNoise * BASIC_TERRAIN_SQUASHING_FACTOR);

    heightOfBlocks = (heightOfBlocks + 1 + BASIC_TERRAIN_SQUASHING_FACTOR) /
                     (1 + BASIC_TERRAIN_SQUASHING_FACTOR + 1 + BASIC_TERRAIN_SQUASHING_FACTOR);

    auto surfaceLevel = static_cast<int>(
        ((heightOfBlocks) * (MAX_HEIGHT_MAP - MINIMAL_TERRAIN_LEVEL)) + MINIMAL_TERRAIN_LEVEL);

    return surfaceLevel;
}

void SimpleTerrainGenerator::generateColumnOfBlocks(ChunkBlocks& chunkBlocks, int surfaceLevel,
                                                    int blockCoordinateX, int globalCoordinateY,
                                                    int blockCoordinateZ)
{
    auto& x = blockCoordinateX;
    auto& z = blockCoordinateZ;

    for (auto y = 0; y < ChunkBlocks::BLOCKS_PER_Y_DIMENSION; ++y)
    {
        auto globalY = globalCoordinateY + y;
        if (globalY == surfaceLevel)
        {
            chunkBlocks.block(x, y, z).setBlockType(BlockId::Grass);
        }
        else if (globalY < surfaceLevel - 5)
        {
            chunkBlocks.block(x, y, z).setBlockType(BlockId::Stone);
        }
        else if (globalY < surfaceLevel)
        {
            chunkBlocks.block(x, y, z).setBlockType(BlockId::Dirt);
        }
        else if (globalY < SEA_LEVEL + 1 && globalY < surfaceLevel + 2)
        {
            chunkBlocks.block(x, y, z).setBlockType(BlockId::Sand);

            // TODO: Change it to more sophisticated system
            if (chunkBlocks.block(x, y - 1, z).id() == BlockId::Grass)
            {
                chunkBlocks.block(x, y - 1, z).setBlockType(BlockId::Sand);
            }
        }
        else if (globalY < SEA_LEVEL)
        {
            chunkBlocks.block(x, y, z).setBlockType(BlockId::Water);
        }
    }
}

int SimpleTerrainGenerator::randomSeed()
{
    static std::random_device r;
    static std::default_random_engine e1(r());

    std::uniform_int_distribution<int> uniformDist(INT_MIN, INT_MAX);
    return uniformDist(e1);
}

}// namespace Voxino