#include "RaycastSingleChunkColoredVoxels.h"
#include "pch.h"

namespace Voxino
{

RaycastSingleChunkColoredVoxels::RaycastSingleChunkColoredVoxels(StateStack& stack,
                                                                 WindowToRender& window,
                                                                 const std::string& shaderName)
    : State(stack)
    , mWindow(window)
    , mPlayer(window)
    , mRenderer(mWindow)
    , mShader{{ShaderType::VertexShader, "resources/Shaders/Raycasting/" + shaderName + ".vs"},
              {ShaderType::FragmentShader, "resources/Shaders/Raycasting/" + shaderName + ".fs"},
              {ShaderType::GeometryShader, "resources/Shaders/Raycasting/" + shaderName + ".gs"}}
    , mTexturePack("default")
    , mVoxelsGpu(128, 128, 128)
{
    Mouse::lockMouseAtCenter(mWindow);

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}


void RaycastSingleChunkColoredVoxels::draw(sf::Window& target) const
{
    MEASURE_SCOPE_WITH_GPU;
    mSkybox.draw(mPlayer.camera());
    mVoxelsGpu.draw(mRenderer, mShader, mPlayer.camera());
    mRenderer.drawRaycast(mShader, mPlayer.camera());
    mPlayer.draw(mRenderer);
}


bool RaycastSingleChunkColoredVoxels::handleEvent(const sf::Event& event)
{
    MEASURE_SCOPE;
    mPlayer.handleEvent(event);
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Escape: Mouse::unlockMouse(mWindow); break;
            default:;// nothing
        }
    }
    Mouse::handleFirstPersonBehaviour(event, mWindow);

    return true;
}


bool RaycastSingleChunkColoredVoxels::fixedUpdate(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.fixedUpdate(deltaTime);
    return true;
}


bool RaycastSingleChunkColoredVoxels::updateImGui(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.camera().updateImGui();
    return true;
}


bool RaycastSingleChunkColoredVoxels::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.update(deltaTime);
    return true;
}

}// namespace Voxino