#include "Resources/TexturePackArray.h"
#include "World/Polygons/Chunks/Types/ChunkBinaryGreedyMeshing.h"
#include "World/Polygons/Chunks/Types/ChunkCulling.h"
#include "World/Polygons/Chunks/Types/ChunkCullingGpu.h"
#include "World/Polygons/Chunks/Types/ChunkGreedyMeshing.h"
#include "World/Polygons/Chunks/Types/ChunkNaive.h"
#include "defines.h"

#include <benchmark/benchmark.h>

namespace Voxino
{

static void BM_ChunkCullingRebuildMesh(benchmark::State& state)
{
    initializeOpenGL();

    auto blockPosition = Block::Coordinate{0, (SimpleTerrainGenerator::MAX_HEIGHT_MAP / 4), 0};
    auto texturePack = TexturePackArray("default");
    auto chunk = Polygons::ChunkCulling(blockPosition, texturePack);
    for (auto _: state)
    {
        chunk.rebuildMesh();
    }
}

BENCHMARK(BM_ChunkCullingRebuildMesh);

static void BM_ChunkCullingGPURebuildMesh(benchmark::State& state)
{
    initializeOpenGL();

    auto blockPosition = Block::Coordinate{0, (SimpleTerrainGenerator::MAX_HEIGHT_MAP / 4), 0};
    auto texturePack = TexturePackArray("default");
    auto chunk = Polygons::ChunkCullingGpu(blockPosition, texturePack);
    for (auto _: state)
    {
        chunk.rebuildMesh();
    }
}

BENCHMARK(BM_ChunkCullingGPURebuildMesh);

static void BM_ChunkNaiveRebuildMesh(benchmark::State& state)
{
    initializeOpenGL();

    auto blockPosition = Block::Coordinate{0, (SimpleTerrainGenerator::MAX_HEIGHT_MAP / 4), 0};
    auto texturePack = TexturePackArray("default");
    auto chunk = Polygons::ChunkNaive(blockPosition, texturePack);
    for (auto _: state)
    {
        chunk.rebuildMesh();
    }
}

BENCHMARK(BM_ChunkNaiveRebuildMesh);

static void BM_ChunkGreedyMeshingRebuildMesh(benchmark::State& state)
{
    initializeOpenGL();

    auto blockPosition = Block::Coordinate{0, (SimpleTerrainGenerator::MAX_HEIGHT_MAP / 4), 0};
    auto texturePack = TexturePackArray("default");
    auto chunk = Polygons::ChunkGreedyMeshing(blockPosition, texturePack);
    for (auto _: state)
    {
        chunk.rebuildMesh();
    }
}

BENCHMARK(BM_ChunkGreedyMeshingRebuildMesh);

static void BM_ChunkBinaryGreedyMeshingRebuildMesh(benchmark::State& state)
{
    initializeOpenGL();

    auto blockPosition = Block::Coordinate{0, (SimpleTerrainGenerator::MAX_HEIGHT_MAP / 4), 0};
    auto texturePack = TexturePackArray("default");
    auto chunk = Polygons::ChunkBinaryGreedyMeshing(blockPosition, texturePack);
    for (auto _: state)
    {
        chunk.rebuildMesh();
    }
}

BENCHMARK(BM_ChunkBinaryGreedyMeshingRebuildMesh);

}// namespace Voxino