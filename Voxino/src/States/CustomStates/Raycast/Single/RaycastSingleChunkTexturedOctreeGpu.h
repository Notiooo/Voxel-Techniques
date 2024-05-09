#pragma once

#include "RaycastSingleChunkBase.h"
#include <World/Raycast/Chunks/Types/RaycastChunkOctreeGpu.h>

namespace Voxino
{

class RaycastSingleChunkTexturedOctreeGpu
    : public RaycastSingleChunkBase<Raycast::RaycastChunkOctreeGpu>
{
public:
    RaycastSingleChunkTexturedOctreeGpu(
        StateStack& stack, WindowToRender& window,
        const std::string& shaderName = SHADER_FILE_NAME(ChunkTexturedVoxelsOctreeGpu));
};
}// namespace Voxino