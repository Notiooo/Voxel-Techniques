#pragma once

#include "Player/Player.h"
#include "Renderer/Renderer.h"
#include "States/State.h"
#include "Utils/FpsCounter.h"
#include "Utils/Mouse.h"
#include "World/Chunks/ChunkContainerPolygons.h"
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
class PolygonMultiChunkState : public State
{
public:
    PolygonMultiChunkState(StateStack& stack, WindowToRender& window,
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
    /**
     * \brief Switches (turn off/on) the wireframe mode
     */
    void switchWireframe();


private:
    WindowToRender& mWindow;
    Player mPlayer;
    Renderer mRenderer;
    Shader mShader;
    TexturePackArray mTexturePack;
    ChunkContainerPolygons<ChunkType> mChunkContainer;
    Skybox mSkybox;
    bool mIsWireframe{false};
};

template<typename ChunkType>
PolygonMultiChunkState<ChunkType>::PolygonMultiChunkState(StateStack& stack, WindowToRender& window,
                                                          const std::string& shaderName)
    : State(stack)
    , mWindow(window)
    , mPlayer(window)
    , mRenderer(mWindow)
    , mShader{{ShaderType::VertexShader, "resources/Shaders/Polygons/" + shaderName + ".vs"},
              {ShaderType::FragmentShader, "resources/Shaders/Polygons/" + shaderName + ".fs"},
              {ShaderType::GeometryShader, "resources/Shaders/Polygons/" + shaderName + ".gs"}}
    , mTexturePack("default")
    , mChunkContainer(mTexturePack)
{
    Mouse::lockMouseAtCenter(mWindow);


    auto radius =
        ChunkBlocks::BLOCKS_PER_DIMENSION * (ChunkContainerBase::CHUNK_RADIUS * 2 + 1) + 2;
    auto center = glm::vec3(ChunkBlocks::BLOCKS_PER_X_DIMENSION / 2.f, 0,
                            ChunkBlocks::BLOCKS_PER_Z_DIMENSION / 2.f);
    center.y +=
        (SimpleTerrainGenerator::MAX_HEIGHT_MAP / 4) + ChunkBlocks::BLOCKS_PER_Y_DIMENSION / 3.f;

    auto cameraOffset = glm::vec3(0.0f,
                                  ChunkBlocks::BLOCKS_PER_Y_DIMENSION *
                                      (std::min(ChunkContainerBase::CHUNK_RADIUS * 2,
                                                ChunkContainerBase::MAX_CHUNKS_IN_HEIGHT)) *
                                      0.66f,
                                  0.0f);
    mPlayer.camera().setRotationPath(radius, center, cameraOffset);

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    spdlog::info("Number of chunks: {}", mChunkContainer.size());
    spdlog::info("Number of chunk vertices: {}", mChunkContainer.numberOfVertices());
    spdlog::info("Size in buffer: {} bytes", mChunkContainer.memorySize());
    FPSCounter::instance().enable();

    mPlayer.camera().setAutomatic(true);
}

template<typename ChunkType>
void PolygonMultiChunkState<ChunkType>::draw(sf::Window& target) const
{
    MEASURE_SCOPE_WITH_GPU;
    if (not mIsWireframe)
    {
        mSkybox.draw(mPlayer.camera());
    }

    mChunkContainer.draw(mRenderer, mShader, mPlayer.camera());
    mPlayer.draw(mRenderer);
}

template<typename ChunkType>
bool PolygonMultiChunkState<ChunkType>::handleEvent(const sf::Event& event)
{
    MEASURE_SCOPE;
    mPlayer.handleEvent(event);
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Escape: Mouse::unlockMouse(mWindow); break;
            case sf::Keyboard::F2: switchWireframe(); break;
            default:;// nothing
        }
    }
    Mouse::handleFirstPersonBehaviour(event, mWindow);

    return true;
}

template<typename ChunkType>
void PolygonMultiChunkState<ChunkType>::switchWireframe()
{
    mIsWireframe = !mIsWireframe;
    spdlog::info("Wireframe mode is {}", (mIsWireframe ? "enabled" : "disabled"));
    if (mIsWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

template<typename ChunkType>
bool PolygonMultiChunkState<ChunkType>::fixedUpdate(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.fixedUpdate(deltaTime);
    // mChunkContainer.fixedUpdate(deltaTime);
    return true;
}

template<typename ChunkType>
bool PolygonMultiChunkState<ChunkType>::updateImGui(const float& deltaTime)
{
    MEASURE_SCOPE;
    ImGui::Begin("Debug");
    if (ImGui::Button("Switch wireframe"))
    {
        switchWireframe();
    }
    ImGui::End();
    mPlayer.updateImGui();
    return true;
}

template<typename ChunkType>
bool PolygonMultiChunkState<ChunkType>::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    static auto totalTime = 0.f;
    totalTime += deltaTime;
    if (totalTime > 60.f)
    {
        requestPush(State_ID::ExitApplicationState);
    }
    mPlayer.update(deltaTime);
    return true;
}

}// namespace Voxino