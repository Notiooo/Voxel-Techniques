#pragma once
#include "States/StateStack.h"
#include <SFML/Window/Event.hpp>
#include <gmock/gmock.h>

namespace Voxino
{

class MockStateStack : public StateStack
{
public:
    MOCK_METHOD(void, draw, (sf::RenderTarget & target, sf::RenderStates states), (const));
    MOCK_METHOD(void, fixedUpdate, (const float& deltaTime));
    MOCK_METHOD(void, update, ());
    MOCK_METHOD(void, handleEvent, (const sf::Event& event));
    MOCK_METHOD(void, push, (State_ID stateID));
    MOCK_METHOD(void, pop, ());
    MOCK_METHOD(void, clear, ());
    MOCK_METHOD(bool, empty, (), (const, noexcept));
};

}// namespace Voxino