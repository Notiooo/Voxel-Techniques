#include "RaycastSingleChunkTexturedVoxelsFixedStep.h"
#include "pch.h"

namespace Voxino
{
RaycastSingleChunkTexturedVoxelsFixedStep::RaycastSingleChunkTexturedVoxelsFixedStep(
    StateStack& stack, WindowToRender& window, const std::string& shaderName)
    : RaycastSingleChunkBase(stack, window, shaderName)
{
}
bool RaycastSingleChunkTexturedVoxelsFixedStep::updateImGui(const float& deltaTime)
{
    if (ImGui::SliderFloat("Fixed size ray", &mFixedSizeRay, 0.01f, 1.f))
    {
        mShader.bind();
        mShader.setUniform("u_FixedStepSize", mFixedSizeRay);
        spdlog::info("New fixed size ray: {}", mFixedSizeRay);
    }
    if (ImGui::Button("Switch algorithm"))
    {
        mSwitchAlgorithm = !mSwitchAlgorithm;
        mShader.bind();
        mShader.setUniform("u_SwitchAlgorithm", mSwitchAlgorithm);
    }
    return RaycastSingleChunkBase<Raycast::RaycastChunk>::updateImGui(deltaTime);
}
}// namespace Voxino