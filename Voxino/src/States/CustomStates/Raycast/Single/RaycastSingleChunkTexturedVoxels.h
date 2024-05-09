#pragma once

#include "RaycastSingleChunkBase.h"
#include <World/Raycast/Chunks/Types/RaycastChunk.h>

namespace Voxino
{

class RaycastSingleChunkTexturedVoxels : public RaycastSingleChunkBase<Raycast::RaycastChunk>
{
public:
    RaycastSingleChunkTexturedVoxels(
        StateStack& stack, WindowToRender& window,
        const std::string& shaderName = SHADER_FILE_NAME(ChunkTexturedVoxels));
};
}// namespace Voxino