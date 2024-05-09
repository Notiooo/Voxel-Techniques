#pragma once

#include "RaycastSingleChunkBase.h"
#include "World/Raycast/Chunks/Types/RaycastChunkBrickmapGpu.h"

namespace Voxino
{

class RaycastSingleChunkTexturedBrickmapGpu
    : public RaycastSingleChunkBase<Raycast::RaycastChunkBrickmapGpu>
{
public:
    RaycastSingleChunkTexturedBrickmapGpu(
        StateStack& stack, WindowToRender& window,
        const std::string& shaderName = SHADER_FILE_NAME(ChunkTexturedVoxelsBrickmapGpu));
};

}// namespace Voxino