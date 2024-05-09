#include "RaycastSingleChunkTexturedOctreeGpu.h"
#include "pch.h"

namespace Voxino
{
RaycastSingleChunkTexturedOctreeGpu::RaycastSingleChunkTexturedOctreeGpu(
    StateStack& stack, WindowToRender& window, const std::string& shaderName)
    : RaycastSingleChunkBase(stack, window, shaderName)
{
}
}// namespace Voxino