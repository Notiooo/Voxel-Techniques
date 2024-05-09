#include "RaycastSingleChunkTexturedVoxels.h"
#include "pch.h"

namespace Voxino
{
RaycastSingleChunkTexturedVoxels::RaycastSingleChunkTexturedVoxels(StateStack& stack,
                                                                   WindowToRender& window,
                                                                   const std::string& shaderName)
    : RaycastSingleChunkBase(stack, window, shaderName)
{
}
}// namespace Voxino