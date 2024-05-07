#include "RaycastSingleChunkTexturedVoxels.h"
#include "pch.h"

namespace Voxino
{


RaycastSingleChunkTexturedVoxels::RaycastSingleChunkTexturedVoxels(StateStack& stack,
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
    , mChunk({0, 128, 0}, mTexturePack)
{
    Mouse::lockMouseAtCenter(mWindow);

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}


void RaycastSingleChunkTexturedVoxels::draw(sf::Window& target) const
{
    MEASURE_SCOPE_WITH_GPU;
    mSkybox.draw(mPlayer.camera());
    mTexturePack.bind(TexturePackArray::Spritesheet::Blocks);
    mChunk.draw(mRenderer, mShader, mPlayer.camera());
    mPlayer.draw(mRenderer);
}


bool RaycastSingleChunkTexturedVoxels::handleEvent(const sf::Event& event)
{
    MEASURE_SCOPE;
    mPlayer.handleEvent(event);
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Escape: Mouse::unlockMouse(mWindow); break;
            case sf::Keyboard::F2: switchVisiblitySphere(); break;
            default:;// nothing
        }
    }
    Mouse::handleFirstPersonBehaviour(event, mWindow);

    return true;
}


bool RaycastSingleChunkTexturedVoxels::fixedUpdate(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.fixedUpdate(deltaTime);
    return true;
}


bool RaycastSingleChunkTexturedVoxels::updateImGui(const float& deltaTime)
{
    MEASURE_SCOPE;
    return true;
}


bool RaycastSingleChunkTexturedVoxels::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.update(deltaTime);
    return true;
}

void RaycastSingleChunkTexturedVoxels::switchVisiblitySphere()
{
    mShader.bind();
    mVisibilitySphereEnabled = !mVisibilitySphereEnabled;
    spdlog::info("Visibility sphere is {}", (mVisibilitySphereEnabled ? "enabled" : "disabled"));
    mShader.setUniform("u_VisibilitySphereEnabled", mVisibilitySphereEnabled);
    mShader.unbind();
}

}// namespace Voxino