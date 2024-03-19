#include "Resources/TexturePack.h"
#include "Resources/TexturePackArray.h"
#include "World/Polygons/Chunks/Types/ChunkCulling.h"
#include "World/Polygons/Chunks/Types/ChunkGreedyMeshing.h"
#include "World/Polygons/Chunks/Types/ChunkNaive.h"
#include "defines.h"
#include <benchmark/benchmark.h>

namespace Voxino
{

static void BM_ChunkCullingRebuildMesh(benchmark::State& state)
{
    initializeOpenGL();

    auto blockPosition = Block::Coordinate{0, 0, 0};
    auto texturePack = TexturePackArray("default");
    auto chunk = ChunkCulling(blockPosition, texturePack);
    for (auto _: state)
    {
        chunk.rebuildMesh();
    }
}

BENCHMARK(BM_ChunkCullingRebuildMesh);

static void BM_ChunkNaiveRebuildMesh(benchmark::State& state)
{
    initializeOpenGL();

    auto blockPosition = Block::Coordinate{0, 0, 0};
    auto texturePack = TexturePackArray("default");
    auto chunk = ChunkNaive(blockPosition, texturePack);
    for (auto _: state)
    {
        chunk.rebuildMesh();
    }
}

BENCHMARK(BM_ChunkNaiveRebuildMesh);

static void BM_ChunkGreedyMeshingRebuildMesh(benchmark::State& state)
{
    initializeOpenGL();

    auto blockPosition = Block::Coordinate{0, 0, 0};
    auto texturePack = TexturePackArray("default");
    auto chunk = ChunkGreedyMeshing(blockPosition, texturePack);
    for (auto _: state)
    {
        chunk.rebuildMesh();
    }
}

BENCHMARK(BM_ChunkGreedyMeshingRebuildMesh);

}// namespace Voxino