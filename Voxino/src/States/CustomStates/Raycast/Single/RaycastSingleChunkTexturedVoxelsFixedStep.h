#pragma once

#include "RaycastSingleChunkBase.h"
#include <World/Raycast/Chunks/Types/RaycastChunk.h>

namespace Voxino
{

class RaycastSingleChunkTexturedVoxelsFixedStep
    : public RaycastSingleChunkBase<Raycast::RaycastChunk>
{
public:
    RaycastSingleChunkTexturedVoxelsFixedStep(
        StateStack& stack, WindowToRender& window,
        const std::string& shaderName = SHADER_FILE_NAME(ChunkTexturedVoxelsFixedStep));


    /**
     * \brief Updates the imgui logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the application was last updated.
     */
    bool updateImGui(const float& deltaTime) override;

private:
    float mFixedSizeRay = 0.1;
    bool mSwitchAlgorithm = false;
};
}// namespace Voxino