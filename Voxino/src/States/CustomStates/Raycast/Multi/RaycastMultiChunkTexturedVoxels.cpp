#include "RaycastMultiChunkTexturedVoxels.h"
#include "pch.h"

#include <Utils/FpsCounter.h>

namespace Voxino
{


RaycastMultiChunkTexturedVoxels::RaycastMultiChunkTexturedVoxels(StateStack& stack,
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


void RaycastMultiChunkTexturedVoxels::draw(sf::Window& target) const
{
    MEASURE_SCOPE_WITH_GPU;
    mSkybox.draw(mPlayer.camera());
    mTexturePack.bind(TexturePackArray::Spritesheet::Blocks);
    mChunkContainer.draw(mRenderer, mShader, mPlayer.camera());
    mPlayer.draw(mRenderer);
}


bool RaycastMultiChunkTexturedVoxels::handleEvent(const sf::Event& event)
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


bool RaycastMultiChunkTexturedVoxels::fixedUpdate(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.fixedUpdate(deltaTime);
    return true;
}


bool RaycastMultiChunkTexturedVoxels::updateImGui(const float& deltaTime)
{
    MEASURE_SCOPE;
    // mChunkContainer.updateImGui();
    unsigned long sum = 0;
    for (auto& [coordinate, chunk]: mChunkContainer.data())
    {
        sum += chunk->lastNumberOfRayIterations();
    }

    ImGui::Begin("Ray Iterations");
    ImGui::Text("Rays: %d", sum);
    ImGui::End();
    ImGui::Begin("Ray");
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
    if (ImGui::SliderInt("Max Ray Iterations Visualization", &mMaxRayVisualization, 1, 100))
    {
        mShader.bind();
        mShader.setUniform("u_MaxRayIterationsVisualization", mMaxRayVisualization);
    }
    ImGui::End();

    mPlayer.updateImGui();
    return true;
}


bool RaycastMultiChunkTexturedVoxels::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    static auto totalTime = 0.f;
    totalTime += deltaTime;
    if (totalTime > 60.f)
    {
        requestPush(State_ID::ExitApplicationState);
    }
    mPlayer.update(deltaTime);
    mChunkContainer.update(deltaTime);
    return true;
}

void RaycastMultiChunkTexturedVoxels::switchVisiblitySphere()
{
    mShader.bind();
    mVisibilitySphereEnabled = !mVisibilitySphereEnabled;
    spdlog::info("Visibility sphere is {}", (mVisibilitySphereEnabled ? "enabled" : "disabled"));
    mShader.setUniform("u_VisibilitySphereEnabled", mVisibilitySphereEnabled);
    mShader.unbind();
}

void RaycastMultiChunkTexturedVoxels::switchRayIterations()
{
    mShader.bind();
    mRayIterationsEnabled = !mRayIterationsEnabled;
    spdlog::info("Ray iterations are {}", (mRayIterationsEnabled ? "enabled" : "disabled"));
    mShader.setUniform("u_DrawRayIterations", mRayIterationsEnabled);
    mShader.unbind();
}

}// namespace Voxino