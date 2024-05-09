#include "Mouse.h"
#include "pch.h"

#include <SFML/Window/Window.hpp>

namespace Voxino
{
void Mouse::centerMouse(const sf::Window& window)
{
    sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);
}

void Mouse::lockMouseAtCenter(sf::Window& window)
{
    if (!isMouseLocked())
    {
        mLockedMouse = true;
        window.setMouseCursorVisible(false);
        centerMouse(window);
    }
}

void Mouse::unlockMouse(sf::Window& window)
{
    if (isMouseLocked())
    {
        window.setMouseCursorVisible(true);
        mLockedMouse = false;
    }
}

bool Mouse::isMouseLocked()
{
    return mLockedMouse;
}

sf::Vector2i Mouse::mouseOffset()
{
    return mMouseOffset;
}

void Mouse::update(const float& deltaTime, const sf::Window& window)
{
    static auto accumulatedDeltaTime = 0.f;
    accumulatedDeltaTime += deltaTime;
    if (isMouseLocked())
    {
        const auto windowCenter = sf::Vector2i(window.getSize().x / 2.f, window.getSize().y / 2.f);

        if (accumulatedDeltaTime > 0.01f)
        {
            mMouseOffset = sf::Mouse::getPosition(window) - windowCenter;
            centerMouse(window);
            accumulatedDeltaTime = 0;
        }
    }
}

void Mouse::handleFirstPersonBehaviour(const sf::Event& event, WindowToRender& applicationWindow)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.key.code == sf::Mouse::Left)
        {
#ifndef DISABLE_IMGUI
            if (!ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) && !ImGui::IsAnyItemActive())
            {
                lockMouseAtCenter(applicationWindow);
            }
#endif
        }
    }
    else if (event.type == sf::Event::GainedFocus)
    {
        lockMouseAtCenter(applicationWindow);
    }
}
}// namespace Voxino