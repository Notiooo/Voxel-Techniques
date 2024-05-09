#include "Player.h"
#include "Utils/Mouse.h"
#include "pch.h"

namespace Voxino
{
Player::Player(WindowToRender& window)
    : mCamera(window)
    , mGameWindow(window)
    , mCrosshairTexture("resources/Textures/crosshair.png")
    , mCrosshair(mCrosshairTexture)
{
    mCrosshair.setPosition({window.getSize().x / 2.f, window.getSize().y / 2.f},
                           Sprite2D::Origin::Center);
    mCrosshair.setOpacity(0.8f);
    mCamera.cameraPosition({mPosition.x, mPosition.y, mPosition.z});
}

void Player::draw(const Renderer& target) const
{
    MEASURE_SCOPE_WITH_GPU;
    mCrosshair.draw(target);
}

void Player::update(const float& deltaTime)
{
    mCamera.cameraPosition({mPosition.x, mPosition.y, mPosition.z});
    mCamera.update(deltaTime);
}


void Player::updatePhysics(float deltaTime)
{
    handleMovementKeyboardInputs(deltaTime);
    decelerateVelocity(deltaTime);
    limitVelocity(deltaTime);
    mPosition += mVelocity;
}

void Player::decelerateVelocity(const float& deltaTime)
{
    const auto decayFactor = exp(-PLAYER_FLYING_DECELERATE_RATIO * deltaTime);
    mVelocity *= decayFactor;
}

void Player::limitVelocity(const float& deltaTime)
{
    static auto limitVelocitySpeed = [](auto& velocity, const auto& maxVelocity)
    {
        if (velocity > maxVelocity)
        {
            velocity = maxVelocity;
        }
        else if (-velocity > maxVelocity)
        {
            velocity = -maxVelocity;
        }
    };

    const auto playerVelocity = PLAYER_MAX_FLYING_SPEED * deltaTime;
    limitVelocitySpeed(mVelocity.x, playerVelocity);
    limitVelocitySpeed(mVelocity.y, playerVelocity);
    limitVelocitySpeed(mVelocity.z, playerVelocity);
}

void Player::fixedUpdate(const float& deltaTime)
{
    updatePhysics(deltaTime);
}


void Player::handleMovementKeyboardInputs(const float& deltaTime)
{
    if (not mGameWindow.hasFocus() or not Mouse::isMouseLocked())
    {
        return;
    }
    constexpr auto ACCELERATION_RATIO = 0.1f;
    auto direction = glm::vec3(0.0f, 0.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        direction += mCamera.directionWithoutPitch();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        direction -= mCamera.directionWithoutPitch();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        direction += mCamera.rightDirectionWithoutPitch();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        direction -= mCamera.rightDirectionWithoutPitch();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) or
        sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        direction += mCamera.upwardDirection();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) or
        sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        direction -= mCamera.upwardDirection();
    }

    if (glm::length(direction) > 0.0f)
    {
        direction = glm::normalize(direction);// Normalize the combined direction
        const auto finalSpeed = PLAYER_ACCELERATE_SPEED * ACCELERATION_RATIO * deltaTime;
        mVelocity += finalSpeed * direction;
    }
}

void Player::handleEvent(const sf::Event& event)
{
    // nothing yet
}

AutomaticCamera& Player::camera()
{
    return mCamera;
}

const AutomaticCamera& Player::camera() const
{
    return mCamera;
}

void Player::updateImGui()
{
    mCamera.updateImGui();
}

}// namespace Voxino