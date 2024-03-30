#pragma once

#include "Player/Player.h"
#include "Renderer/Renderer.h"
#include "States/State.h"
#include "Utils/Mouse.h"
#include "World/InfiniteGridFloor.h"
#include "World/Polygons/Chunks/Types/ChunkCulling.h"
#include "World/Skybox.h"

namespace Voxino
{

class StateStack;

/**
 * \brief The game state in which the game world is created,
 * all objects are placed and the processes inside the game world are controlled.
 */
template<typename ChunkType>
class PolygonSingleChunkState : public State
{
public:
    PolygonSingleChunkState(StateStack& stack, WindowToRender& window,
                            const std::string& shaderName = "Chunk");

    /**
     * \brief Draws only this state to the passed target
     * \param target where it should be drawn to
     */
    void draw(sf::Window& target) const override;

    /**
     * \brief Updates the state logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     */
    bool fixedUpdate(const float& deltaTime) override;

    /**
     * \brief Updates the imgui logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the application was last updated.
     */
    bool updateImGui(const float& deltaTime) override;

    /**
     * \brief Updates the game logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the game was last updated.
     */
    bool update(const float& deltaTime) override;

    /**
     * \brief It takes input (event) from the user and interprets it
     * \param event user input
     */
    bool handleEvent(const sf::Event& event) override;

private:
    void switchWireframe();


private:
    WindowToRender& mWindow;
    Player mPlayer;
    Renderer mRenderer;
    Shader mShader;
    TexturePackArray mTexturePack;
    ChunkType mChunk;
    Skybox mSkybox;
};

template<typename ChunkType>
PolygonSingleChunkState<ChunkType>::PolygonSingleChunkState(StateStack& stack,
                                                            WindowToRender& window,
                                                            const std::string& shaderName)
    : State(stack)
    , mWindow(window)
    , mPlayer(window)
    , mRenderer(mWindow)
    , mShader{{ShaderType::VertexShader, "resources/Shaders/Polygons/" + shaderName + ".vs"},
              {ShaderType::FragmentShader, "resources/Shaders/Polygons/" + shaderName + ".fs"}}
    , mTexturePack("default")
    , mChunk({0, -72, 0}, mTexturePack)
{
    Mouse::lockMouseAtCenter(mWindow);

    // GLCall(glEnable(GL_CULL_FACE));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    spdlog::info("Number of chunk vertices: {}", mChunk.numberOfVertices());
    spdlog::info("Size in buffer: {} bytes", mChunk.memorySize());
}

template<typename ChunkType>
void PolygonSingleChunkState<ChunkType>::draw(sf::Window& target) const
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

template<typename ChunkType>
bool PolygonSingleChunkState<ChunkType>::handleEvent(const sf::Event& event)
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

template<typename ChunkType>
void PolygonSingleChunkState<ChunkType>::switchWireframe()
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

template<typename ChunkType>
bool PolygonSingleChunkState<ChunkType>::fixedUpdate(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.fixedUpdate(deltaTime);
    mChunk.fixedUpdate(deltaTime);
    return true;
}

template<typename ChunkType>
bool PolygonSingleChunkState<ChunkType>::updateImGui(const float& deltaTime)
{
    MEASURE_SCOPE;
    return true;
}

template<typename ChunkType>
bool PolygonSingleChunkState<ChunkType>::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.update(deltaTime);
    return true;
}

}// namespace Voxino