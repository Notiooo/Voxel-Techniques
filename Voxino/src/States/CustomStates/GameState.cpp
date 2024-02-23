#include "GameState.h"
#include "States/StateStack.h"
#include "Utils/Mouse.h"
#include "pch.h"

#include <Player/Player.h>

namespace Voxino
{

GameState::GameState(StateStack& stack, WindowToRender& window)
    : State(stack)
    , mWindow(window)
    , mPlayer(window)
    , mRenderer(mWindow)
    , mGameBackground(glm::vec2(1280, 720.f), glm::vec4(0.85f, 0.85f, 0.85f, 1.f))
    , mPhaseInLogoColor({window.getSize().x, window.getSize().y}, {0.19f, 0.24f, 0.31f, 1.f})
{
    Mouse::lockMouseAtCenter(mWindow);
    mGameBackground.setPosition({0, 0});
    mPhaseInLogoColor.setPosition({0, 0});
    mPhaseInClock.restart();
}

void GameState::draw(sf::Window& target) const
{
    MEASURE_SCOPE_WITH_GPU;
    mGameBackground.draw(mRenderer);
    mSkybox.draw(mPlayer.camera());
    mInfiniteGridFloor.draw(target, mPlayer.camera());
    mPhaseInLogoColor.draw(mRenderer);
    mPlayer.draw(mRenderer);
}

bool GameState::fixedUpdate(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.fixedUpdate(deltaTime);
    return true;
}

bool GameState::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    mPlayer.update(deltaTime);

    if (mPhaseInLogoColor.opacity() > 0)
    {
        mPhaseInLogoColor.setOpacity(mPhaseInLogoColor.opacity() - deltaTime / 4.f);
    }
    return true;
}

bool GameState::handleEvent(const sf::Event& event)
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

bool GameState::updateImGui(const float& deltaTime)
{
    MEASURE_SCOPE;
    return true;
}

}// namespace Voxino