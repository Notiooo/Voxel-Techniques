#include "RaycastSingleChunkTexturedBrickmapGpu.h"
#include "pch.h"

namespace Voxino
{
RaycastSingleChunkTexturedBrickmapGpu::RaycastSingleChunkTexturedBrickmapGpu(
    StateStack& stack, WindowToRender& window, const std::string& shaderName)
    : RaycastSingleChunkBase(stack, window, shaderName)
{
}
}// namespace Voxino