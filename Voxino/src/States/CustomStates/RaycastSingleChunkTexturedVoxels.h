#pragma once

#include <World/Skybox.h>

#include "Player/Player.h"
#include "Renderer/Renderer.h"
#include "Resources/TexturePackArray.h"
#include "States/State.h"
#include "Utils/Mouse.h"
#include "World/InfiniteGridFloor.h"

#include <World/Raycast/Chunks/Types/RaycastChunk.h>

namespace Voxino
{

class StateStack;

/**
 * \brief The game state in which the game world is created,
 * all objects are placed and the processes inside the game world are controlled.
 */
class RaycastSingleChunkTexturedVoxels : public State
{
public:
    RaycastSingleChunkTexturedVoxels(StateStack& stack, WindowToRender& window,
                                     const std::string& shaderName = "ChunkTexturedVoxels");

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
    Raycast::RaycastChunk mChunk;
    Skybox mSkybox;
    bool mVisibilitySphereEnabled = true;
};

}// namespace Voxino