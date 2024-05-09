#pragma once

#include "Player/Player.h"
#include "Renderer/Renderer.h"
#include "Resources/TexturePackArray.h"
#include "States/State.h"
#include "Utils/Mouse.h"
#include "World/InfiniteGridFloor.h"


#include <GL/glew.h>
#include <Utils/FpsCounter.h>
#include <Utils/RayCounters.h>
#include <World/Raycast/Chunks/Types/RaycastChunk.h>
#include <World/Skybox.h>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Voxino
{

#ifdef _DEBUG
    #define SHADER_FILE_NAME(shader) "Debug/" #shader
#else
    #define SHADER_FILE_NAME(shader) "Benchmark/" #shader
#endif

template<typename ChunkType>
class RaycastSingleChunkBase : public State
{
public:
    RaycastSingleChunkBase(StateStack& stack, WindowToRender& window,
                           const std::string& shaderName);

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

    void switchRayIterations();

protected:
    WindowToRender& mWindow;
    Player mPlayer;
    Renderer mRenderer;
    Shader mShader;
    TexturePackArray mTexturePack;
    ChunkType mChunk;
    Skybox mSkybox;
    bool mVisibilitySphereEnabled = true;
    bool mRayIterationsEnabled = false;
    int mMaxRayVisualization = 100;
    bool mDrawNormals = false;
    bool mDrawDepths = false;
    mutable RayCounter mRayCounter;
};

template<typename ChunkType>
RaycastSingleChunkBase<ChunkType>::RaycastSingleChunkBase(StateStack& stack, WindowToRender& window,
                                                          const std::string& shaderName)
    : State(stack)
    , mWindow(window)
    , mPlayer(window)
    , mRenderer(mWindow)
    , mShader{{ShaderType::VertexShader, "resources/Shaders/Raycast/" + shaderName + ".vs"},
              {ShaderType::FragmentShader, "resources/Shaders/Raycast/" + shaderName + ".fs"},
              {ShaderType::GeometryShader, "resources/Shaders/Raycast/" + shaderName + ".gs"}}
    , mTexturePack("default")
    , mChunk({0, (SimpleTerrainGenerator::MAX_HEIGHT_MAP / 3), 0}, mTexturePack)
{
    Mouse::lockMouseAtCenter(mWindow);

    auto radius = ChunkBlocks::BLOCKS_PER_DIMENSION + 2;
    auto center = glm::vec3(ChunkBlocks::BLOCKS_PER_X_DIMENSION / 2.f, 0,
                            ChunkBlocks::BLOCKS_PER_Z_DIMENSION / 2.f);
    center.y +=
        (SimpleTerrainGenerator::MAX_HEIGHT_MAP / 4) + ChunkBlocks::BLOCKS_PER_Y_DIMENSION / 3.f;

    auto cameraOffset = glm::vec3(0.0f, ChunkBlocks::BLOCKS_PER_Y_DIMENSION * 0.66f, 0.0f);
    mPlayer.camera().setRotationPath(radius, center, cameraOffset);
    // mPlayer.camera().setStaticPointWithAngle(center, radius, cameraOffset, 225.f);

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    FPSCounter::instance().enable();

    mPlayer.camera().setAutomatic(true);
}

template<typename ChunkType>
void RaycastSingleChunkBase<ChunkType>::draw(sf::Window& target) const
{
    MEASURE_SCOPE_WITH_GPU;
    if (not mRayIterationsEnabled)
    {
        mSkybox.draw(mPlayer.camera());
    }
    mTexturePack.bind(TexturePackArray::Spritesheet::Blocks);
    mShader.bind();
    mShader.setUniform("u_TextureArray", 0);
    mChunk.draw(mRenderer, mShader, mPlayer.camera());
    mPlayer.draw(mRenderer);
}

template<typename ChunkType>
bool RaycastSingleChunkBase<ChunkType>::handleEvent(const sf::Event& event)
{
    MEASURE_SCOPE;
    mPlayer.handleEvent(event);
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
            case sf::Keyboard::Escape: Mouse::unlockMouse(mWindow); break;
            case sf::Keyboard::F2: switchVisiblitySphere(); break;
            case sf::Keyboard::F3: switchRayIterations(); break;
            default:;// nothing
        }
    }
    Mouse::handleFirstPersonBehaviour(event, mWindow);

    return true;
}

template<typename ChunkType>
bool RaycastSingleChunkBase<ChunkType>::fixedUpdate(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.fixedUpdate(deltaTime);
    return true;
}

template<typename ChunkType>
bool RaycastSingleChunkBase<ChunkType>::updateImGui(const float& deltaTime)
{
    MEASURE_SCOPE;
    ImGui::Begin("Debug");
    auto switchFlag = [this](bool& flag, const std::string& name, const std::string& uniformName)
    {
        if (ImGui::Button(name.c_str()))
        {
            flag = !flag;
            spdlog::info("{} is {}", name, (flag ? "enabled" : "disabled"));
            mShader.bind();
            mShader.setUniform(uniformName, flag);
        }
    };
    switchFlag(mDrawNormals, "Switch Normals", "u_DrawNormals");
    switchFlag(mDrawDepths, "Switch Depth", "u_DrawDepth");
    if (ImGui::Button("Switch Ray Visualization"))
    {
        switchRayIterations();
    }
    if (mRayIterationsEnabled)
    {
        if (ImGui::SliderInt("Max Ray Iterations Visualization", &mMaxRayVisualization, 1, 100))
        {
            mShader.bind();
            mShader.setUniform("u_MaxRayIterationsVisualization", mMaxRayVisualization);
        }
    }
    if (ImGui::Button("Switch Visibility Sphere"))
    {
        switchVisiblitySphere();
    }
    ImGui::End();
    mChunk.updateImGui();
    mPlayer.updateImGui();
    return true;
}

template<typename ChunkType>
bool RaycastSingleChunkBase<ChunkType>::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    static auto totalTime = 0.f;
    totalTime += deltaTime;
    if (totalTime > 60.f)
    {
        auto currentState = requestStateOnTop();
        spdlog::info("Allocated memory: {} bytes", mChunk.memorySize());
        mRayCounter.printOverallAverageIterations(toString(currentState));
        mRayCounter.printRayStandardDeviation(toString(currentState));
        requestPush(State_ID::ExitApplicationState);
    }
    mPlayer.update(deltaTime);
    mChunk.update(deltaTime);
    mRayCounter.addMeasurement(mChunk.lastNumberOfRayIterations());
    return true;
}

template<typename ChunkType>
void RaycastSingleChunkBase<ChunkType>::switchVisiblitySphere()
{
    mShader.bind();
    mVisibilitySphereEnabled = !mVisibilitySphereEnabled;
    spdlog::info("Visibility sphere is {}", (mVisibilitySphereEnabled ? "enabled" : "disabled"));
    mShader.setUniform("u_VisibilitySphereEnabled", mVisibilitySphereEnabled);
    mShader.unbind();
}

template<typename ChunkType>
void RaycastSingleChunkBase<ChunkType>::switchRayIterations()
{
    mShader.bind();
    mRayIterationsEnabled = !mRayIterationsEnabled;
    spdlog::info("Ray iterations are {}", (mRayIterationsEnabled ? "enabled" : "disabled"));
    mShader.setUniform("u_DrawRayIterations", mRayIterationsEnabled);
    mShader.unbind();
}

}// namespace Voxino