#include "Camera.h"
#include "pch.h"

#include "Utils/Mouse.h"
#include <SFML/Window/Keyboard.hpp>
#include <Utils/Lerp.h>

namespace Voxino
{
Camera::Camera(const WindowToRender& target)
    : mRenderTarget(target)
{
    mViewMatrix = glm::lookAt(mCameraPosition, mCameraPosition + mCameraFront, mCameraUp);

    const auto targetSize = mRenderTarget.getSize();
    mProjectionMatrix = glm::perspective(
        glm::radians(0.0f), static_cast<float>(targetSize.x / targetSize.y), 1.f, 100.f);
}

void Camera::updateShakeValues(const float& deltaTime)
{
    constexpr auto tolerance = 2.f;
    mCurrentShakeValues = lerp(mCurrentShakeValues, mTargetShakeValues, deltaTime * 6.f);
    auto difference = glm::abs(static_cast<glm::vec3>(mCurrentShakeValues - mTargetShakeValues));
    if (difference.x <= tolerance && difference.y <= tolerance && difference.z <= tolerance)
    {
        mTargetShakeValues = {0, 0, 0};
    }
}

void Camera::update(const float& deltaTime)
{
    updateShakeValues(deltaTime);
    handleMouseInputs(deltaTime);

    const auto width = static_cast<float>(mRenderTarget.getSize().x);
    const auto height = static_cast<float>(mRenderTarget.getSize().y);
    mProjectionMatrix = glm::perspective(glm::radians(mFovCamera), width / height, 0.1f, 10000.f);
    mViewMatrix = glm::lookAt(mCameraPosition, mCameraPosition + mCameraFront, mCameraUp);
}

void Camera::fixedUpdate(const float& deltaTime)
{
    handleKeyboardInputs(deltaTime);
}

void Camera::handleMouseInputs(const float& deltaTime)
{
    if (Mouse::isMouseLocked() && mAreControlsEnabled)
    {
        calculateCameraAngles(deltaTime);
        keepNaturalPitchRanges();
        calculateCameraDirectionVector();
    }
}

void Camera::calculateCameraDirectionVector()
{
    const auto rotation = mRotation + mCurrentShakeValues;
    glm::vec3 direction;
    direction.x = cos(glm::radians(rotation.yaw)) * cos(glm::radians(rotation.pitch));
    direction.y = sin(glm::radians(rotation.pitch));
    direction.z = sin(glm::radians(rotation.yaw)) * cos(glm::radians(rotation.pitch));
    mCameraFront = glm::normalize(direction);

    direction.x = cos(glm::radians(rotation.yaw));
    direction.y = 0;
    direction.z = sin(glm::radians(rotation.yaw));
    mCameraFrontWithoutPitch = glm::normalize(direction);
}

void Camera::calculateCameraAngles(const float& deltaTime)
{
    auto mouseOffset = static_cast<sf::Vector2f>(Mouse::mouseOffset());

    mouseOffset.x *= mCameraSensitivity * deltaTime;
    mouseOffset.y *= mCameraSensitivity * deltaTime * -1;

    mRotation.yaw += mouseOffset.x;
    mRotation.pitch += mouseOffset.y;
}

void Camera::keepNaturalPitchRanges()
{
    if (mRotation.pitch > 89.0f)
    {
        mRotation.pitch = 89.0f;
    }
    if (mRotation.pitch < -89.0f)
    {
        mRotation.pitch = -89.0f;
    }
}

void Camera::handleKeyboardInputs(const float& deltaTime)
{
    auto cameraSpeed = this->mCameraSpeed;
    cameraSpeed = applyAdditionalCameraAcceleration(cameraSpeed);
    handleCameraMovement(deltaTime, cameraSpeed);
}

void Camera::handleCameraMovement(const float& deltaTime, float cameraSpeed)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        mCameraPosition += cameraSpeed * mCameraFront * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        mCameraPosition -= cameraSpeed * mCameraFront * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        mCameraPosition +=
            glm::normalize(glm::cross(mCameraFront, mCameraUp)) * cameraSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        mCameraPosition -=
            glm::normalize(glm::cross(mCameraFront, mCameraUp)) * cameraSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        mCameraPosition -= cameraSpeed * mCameraUp * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        mCameraPosition += cameraSpeed * mCameraUp * deltaTime;
    }
}

float Camera::applyAdditionalCameraAcceleration(float cameraSpeed)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
    {
        cameraSpeed *= 2;
    }
    return cameraSpeed;
}

void Camera::handleEvent(const sf::Event& event)
{
    switch (event.type)
    {
        case sf::Event::MouseWheelMoved: mFovCamera -= event.mouseWheel.delta; break;
        default:;// nothing
    }
}

glm::mat4 Camera::view() const
{
    return mViewMatrix;
}

glm::mat4 Camera::projection() const
{
    return mProjectionMatrix;
}

glm::vec3 Camera::cameraPosition() const
{
    return mCameraPosition;
}

void Camera::updateImGui()
{
    ImGui::Begin("Camera");
    ImGui::SliderFloat3("Translation", &mCameraPosition.x, 0.0f, 960.0f);
    ImGui::End();
}

Rotation3D Camera::rotation() const
{
    return mRotation;
}

void Camera::shake()
{
    mTargetShakeValues = {0.f, 4.f, 0.f};
}

float Camera::fov() const
{
    return mFovCamera;
}

glm::vec3 Camera::direction() const
{
    return mCameraFront;
}

glm::vec3 Camera::rightDirection() const
{
    return glm::cross(mCameraFront, mCameraUp);
}

glm::vec3 Camera::rightDirectionWithoutPitch() const
{
    return glm::cross(mCameraFrontWithoutPitch, mCameraUp);
}

glm::vec3 Camera::upwardDirection() const
{
    return mCameraUp;
}

void Camera::cameraPosition(const glm::vec3& newPosition)
{
    mCameraPosition = newPosition;
}

glm::vec3 Camera::directionWithoutPitch() const
{
    return mCameraFrontWithoutPitch;
}

void Camera::toggleControls()
{
    mAreControlsEnabled = !mAreControlsEnabled;
}
}// namespace Voxino