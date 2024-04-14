#include "RaycastSingleChunkColoredVoxels.h"
#include "Utils/RGBA.h"
#include "pch.h"

namespace Voxino
{

void RaycastSingleChunkColoredVoxels::fillVoxels()
{
    std::vector<RGBA> voxels(128 * 128 * 128);
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0, 255);
    std::uniform_int_distribution<> alphaDistr(0, 99);

    for (auto& voxel: voxels)
    {
        voxel.r = static_cast<GLubyte>(distr(eng));
        voxel.g = static_cast<GLubyte>(distr(eng));
        voxel.b = static_cast<GLubyte>(distr(eng));
        voxel.a = alphaDistr(eng) < 5 ? 255 : 0;// 5% chance for 255
    }

    mVoxelsGpu.fill(voxels);
}
RaycastSingleChunkColoredVoxels::RaycastSingleChunkColoredVoxels(StateStack& stack,
                                                                 WindowToRender& window,
                                                                 const std::string& shaderName)
    : State(stack)
    , mWindow(window)
    , mPlayer(window)
    , mRenderer(mWindow)
    , mShader{{ShaderType::VertexShader, "resources/Shaders/Raycast/" + shaderName + ".vs"},
              {ShaderType::FragmentShader, "resources/Shaders/Raycast/" + shaderName + ".fs"},
              {ShaderType::GeometryShader, "resources/Shaders/Raycast/" + shaderName + ".gs"}}
    , mTexturePack("default")
    , mVoxelsGpu(128, 128, 128)
{
    Mouse::lockMouseAtCenter(mWindow);

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    fillVoxels();
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
    return true;
}


bool RaycastSingleChunkColoredVoxels::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.update(deltaTime);
    return true;
}

}// namespace Voxino