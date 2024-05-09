#pragma once

#include "Player/Player.h"
#include "Renderer/Renderer.h"
#include "Resources/TexturePackArray.h"
#include "States/State.h"
#include "Utils/Mouse.h"
#include "World/InfiniteGridFloor.h"

#include "Utils/RayCounters.h"
#include <Utils/FpsCounter.h>
#include <World/Chunks/ChunkContainerRaycast.h>
#include <World/Raycast/Chunks/Types/RaycastChunk.h>
#include <World/Skybox.h>

namespace Voxino
{

class StateStack;


template<typename ChunkType>
class RaycastMultiChunk : public State
{
public:
    RaycastMultiChunk(StateStack& stack, WindowToRender& window, const std::string& shaderName);

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
     * \brief Switches (turn off/on) the sphere that let camera see through voxels
     */
    void switchVisiblitySphere();

private:
    WindowToRender& mWindow;
    Player mPlayer;
    Renderer mRenderer;
    Shader mShader;
    TexturePackArray mTexturePack;
    ChunkContainerRaycast<ChunkType> mChunkContainer;
    Skybox mSkybox;
    bool mVisibilitySphereEnabled = true;
    float mFixedSizeRay = 0.1;
    bool mSwitchAlgorithm = false;
    bool mDrawNormals = false;
    bool mDrawDepths = false;
    mutable RayCounter mRayCounter;
};

}// namespace Voxino


template<typename ChunkType>
Voxino::RaycastMultiChunk<ChunkType>::RaycastMultiChunk(StateStack& stack, WindowToRender& window,
                                                        const std::string& shaderName)
    : State(stack)
    , mWindow(window)
    , mPlayer(window)
    , mRenderer(mWindow)
    , mShader{{ShaderType::VertexShader, "resources/Shaders/Raycast/" + shaderName + ".vs"},
              {ShaderType::FragmentShader, "resources/Shaders/Raycast/" + shaderName + ".fs"},
              {ShaderType::GeometryShader, "resources/Shaders/Raycast/" + shaderName + ".gs"}}
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
    FPSCounter::instance().enable();

    mPlayer.camera().setAutomatic(true);
}


template<typename ChunkType>
void Voxino::RaycastMultiChunk<ChunkType>::draw(sf::Window& target) const
{
    MEASURE_SCOPE_WITH_GPU;
    mSkybox.draw(mPlayer.camera());
    mTexturePack.bind(TexturePackArray::Spritesheet::Blocks);
    mChunkContainer.draw(mRenderer, mShader, mPlayer.camera());
    mPlayer.draw(mRenderer);
}

template<typename ChunkType>
bool Voxino::RaycastMultiChunk<ChunkType>::handleEvent(const sf::Event& event)
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

template<typename ChunkType>
bool Voxino::RaycastMultiChunk<ChunkType>::fixedUpdate(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.fixedUpdate(deltaTime);
    return true;
}

template<typename ChunkType>
bool Voxino::RaycastMultiChunk<ChunkType>::updateImGui(const float& deltaTime)
{
    MEASURE_SCOPE;
    mChunkContainer.updateImGui();
    unsigned long sum = 0;
    for (auto& [coordinate, chunk]: mChunkContainer.data())
    {
        sum += chunk->lastNumberOfRayIterations();
    }

    ImGui::Begin("Ray Iterations");
    ImGui::Text("Rays: %d", sum);
    ImGui::End();
    ImGui::Begin("Debug");
    auto currentState = requestStateOnTop();
    if (currentState == State_ID::RaycastMultiChunkTexturedVoxelsFixedStep)
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
    }
    if (ImGui::Button("Switch Normals"))
    {
        mDrawNormals = !mDrawNormals;
        mShader.bind();
        mShader.setUniform("u_DrawNormals", mDrawNormals);
    }
    if (ImGui::Button("Switch Depth"))
    {
        mDrawDepths = !mDrawDepths;
        mShader.bind();
        mShader.setUniform("u_DrawDepth", mDrawDepths);
    }
    ImGui::End();

    mPlayer.updateImGui();
    return true;
}

template<typename ChunkType>
bool Voxino::RaycastMultiChunk<ChunkType>::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    static auto totalTime = 0.f;
    totalTime += deltaTime;
    if (totalTime > 60.f)
    {
        unsigned long sum = 0;
        for (auto& [coordinate, chunk]: mChunkContainer.data())
        {
            sum += chunk->memorySize();
        }
        spdlog::info("Allocated memory: {} bytes", sum);
        mRayCounter.printOverallAverageIterations("RaycastMultiChunk");
        mRayCounter.printRayStandardDeviation("RaycastMultiChunk");
        requestPush(State_ID::ExitApplicationState);
    }
    mChunkContainer.update(deltaTime);
    mPlayer.update(deltaTime);
    unsigned long sum = 0;
    for (auto& [coordinate, chunk]: mChunkContainer.data())
    {
        sum += chunk->lastNumberOfRayIterations();
    }
    mRayCounter.addMeasurement(sum);
    return true;
}

template<typename ChunkType>
void Voxino::RaycastMultiChunk<ChunkType>::switchVisiblitySphere()
{
    mShader.bind();
    mVisibilitySphereEnabled = !mVisibilitySphereEnabled;
    spdlog::info("Visibility sphere is {}", (mVisibilitySphereEnabled ? "enabled" : "disabled"));
    mShader.setUniform("u_VisibilitySphereEnabled", mVisibilitySphereEnabled);
    mShader.unbind();
}