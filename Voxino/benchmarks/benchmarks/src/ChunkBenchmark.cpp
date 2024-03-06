#include "Resources/TexturePack.h"
#include "World/Polygons/Chunks/Chunk.h"
#include "defines.h"
#include <benchmark/benchmark.h>

namespace Voxino
{

static void BM_ChunkRebuildMesh(benchmark::State& state)
{
    initializeOpenGL();

    auto blockPosition = Block::Coordinate{0, 0, 0};
    auto texturePack = TexturePack("default");
    auto chunk = Chunk(blockPosition, texturePack);
    for (auto _: state)
    {
        chunk.rebuildMesh();
    }
}

BENCHMARK(BM_ChunkRebuildMesh);
}// namespace Voxino