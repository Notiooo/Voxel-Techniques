#include "LogoState.h"
#include "States/StateStack.h"
#include "pch.h"

namespace Voxino
{

LogoState::LogoState(StateStack& stack, sf::Window& window)
    : State(stack)
    , mWindow(window)
    , mRenderer(mWindow)
    , mLogoTexture("resources/Textures/logo.png")
    , mLogo(mLogoTexture)
    , mPhase(Phase::FadeIn)
    , mRectangle({window.getSize().x, window.getSize().y}, {0.19f, 0.24f, 0.31f, 1.f})
{
    mClock.restart();
    mLogo.setPosition(glm::vec2(mWindow.getSize().x / 2.f, mWindow.getSize().y / 2.f),
                      Sprite2D::Origin::Center);
    mLogo.setHeight(mWindow.getSize().y / 2.f);
    mLogo.setOpacity(0);
    mRectangle.setPosition({0, 0});
}

void LogoState::draw(sf::Window& target) const
{
    MEASURE_SCOPE_WITH_GPU;
    mRectangle.draw(mRenderer);
    mLogo.draw(mRenderer);
}

bool LogoState::fixedUpdate(const float& deltaTime)
{
    MEASURE_SCOPE;
    return true;
}

bool LogoState::update(const float& deltaTime)
{
    MEASURE_SCOPE;
    switch (mPhase)
    {
        case Phase::FadeIn:
            mLogo.setOpacity((mClock.getElapsedTime().asSeconds() / 2.f) - 0.2f);
            if (mClock.getElapsedTime().asSeconds() > 3.f)
            {
                mPhase = Phase::Fadeout;
                mClock.restart();
            }
            break;

        case Phase::Fadeout:
            mLogo.setOpacity(2.f - mClock.getElapsedTime().asSeconds() / 2.f);
            if (mClock.getElapsedTime().asSeconds() > 4.f)
            {
                requestPop();
                requestPush(State_ID::GameState);
            }
            break;
    }
    return true;
}

bool LogoState::handleEvent(const sf::Event& event)
{
    MEASURE_SCOPE;
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
    {
        requestPop();
        requestPush(State_ID::GameState);
    }
    return true;
}

bool LogoState::updateImGui(const float& deltaTime)
{
    MEASURE_SCOPE;
    return true;
}

}// namespace Voxino