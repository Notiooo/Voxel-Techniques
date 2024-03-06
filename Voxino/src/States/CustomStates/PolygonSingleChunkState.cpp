#include "PolygonSingleChunkState.h"
#include "Utils/Mouse.h"
#include "pch.h"

namespace Voxino
{
PolygonSingleChunkState::PolygonSingleChunkState(StateStack& stack, WindowToRender& window)
    : State(stack)
    , mWindow(window)
    , mPlayer(window)
    , mRenderer(mWindow)
    , mShader{{ShaderType::VertexShader, "resources/Shaders/Polygons/Chunk.vs"},
              {ShaderType::FragmentShader, "resources/Shaders/Polygons/Chunk.fs"}}
    , mTexturePack("default")
    , mChunk({0, -72, 0}, mTexturePack)
{
    Mouse::lockMouseAtCenter(mWindow);

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void PolygonSingleChunkState::draw(sf::Window& target) const
{
    MEASURE_SCOPE_WITH_GPU;
    {
        // do not draw skybox in wireframe mode
        std::unique_ptr<int[]> rastMode(new int[2]);
        GLCall(glGetIntegerv(GL_POLYGON_MODE, rastMode.get()));

        if (rastMode[1] == GL_FILL)
        {
            mSkybox.draw(mPlayer.camera());
        }
    }
    mChunk.drawTerrain(mRenderer, mShader, mPlayer.camera());
    mChunk.drawLiquids(mRenderer, mShader, mPlayer.camera());
    mChunk.drawFlorals(mRenderer, mShader, mPlayer.camera());
    mPlayer.draw(mRenderer);
}


bool PolygonSingleChunkState::handleEvent(const sf::Event& event)
{
    MEASURE_SCOPE;
    mPlayer.handleEvent(event);
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            // case sf::Keyboard::Escape: requestPush(State_ID::PauseState); break;
            case sf::Keyboard::Escape: Mouse::unlockMouse(mWindow); break;
            case sf::Keyboard::F1: switchWireframe(); break;
            default:;// nothing
        }
    }
    Mouse::handleFirstPersonBehaviour(event, mWindow);

    return true;
}

void PolygonSingleChunkState::switchWireframe()
{
    std::unique_ptr<int[]> rastMode(new int[2]);
    GLCall(glGetIntegerv(GL_POLYGON_MODE, rastMode.get()));

    if (rastMode[1] == GL_FILL)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

bool PolygonSingleChunkState::fixedUpdate(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.fixedUpdate(deltaTime);
    mChunk.fixedUpdate(deltaTime);
    return true;
}
bool PolygonSingleChunkState::updateImGui(const float& deltaTime)
{
    MEASURE_SCOPE;
    return true;
}

bool PolygonSingleChunkState::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.update(deltaTime);
    return true;
}


}// namespace Voxino