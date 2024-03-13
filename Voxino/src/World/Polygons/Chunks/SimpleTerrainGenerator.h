#pragma once
#include "ChunkBlocks.h"
#include "World/Polygons/Block/Block.h"
#include "World/Polygons/Chunks/Types/Chunk.h"
#include <set>

namespace Voxino
{
class SimpleTerrainGenerator
{
public:
    explicit SimpleTerrainGenerator(int seed = 1337);

    /**
     * @brief Generates terrain for a given chunk with a given set of blocks
     * @param chunk Reference to the chunk on which the terrain is to be generated
     * @param chunkBlocks Collection of blocks of given chunk
     */
    void generateTerrain(ChunkInterface& chunk, ChunkBlocks& chunkBlocks);

    /**
     * @brief Returns a random seed that can be used to generate terrain
     * @return Random int value
     */
    static int randomSeed();

private:
    static constexpr auto SEA_LEVEL = 60;
    static constexpr auto MINIMAL_TERRAIN_LEVEL = 20;
    static constexpr auto BASIC_TERRAIN_SQUASHING_FACTOR = 0.25f;
    static constexpr int MAX_HEIGHT_MAP = ChunkBlocks::BLOCKS_PER_Y_DIMENSION - 10;

    /**
     * @brief Generates terrain on the indicated chunk using the indicated biome.
     * @param chunk Chunk on which the site is to be created.
     * @param chunkBlocks Chunk blocks that are overwritten thus creating terrain.
     */
    void generateTerrainForChunk(const ChunkInterface& chunk, ChunkBlocks& chunkBlocks);

    static void generateColumnOfBlocks(ChunkBlocks& chunkBlocks, int surfaceLevel,
                                       int blockCoordinateX, int blockCoordinateZ);

    int surfaceLevelAtGivenPosition(int blockCoordinateX, int blockCoordinateZ);

private:
    FastNoiseLite mBasicTerrain;
};

}// namespace Voxino