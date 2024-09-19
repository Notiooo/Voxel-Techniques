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

    const auto targetSize = glm::vec2(mRenderTarget.getSize().x, mRenderTarget.getSize().y);
    mProjectionMatrix = glm::perspective(glm::radians(mFovCamera), targetSize.x / targetSize.y,
                                         mNearPlane, mFarPlane);
}

void Camera::update(const float& deltaTime)
{
    handleMouseInputs(deltaTime);

    // const auto width = static_cast<float>(mRenderTarget.getSize().x);
    // const auto height = static_cast<float>(mRenderTarget.getSize().y);
    // mProjectionMatrix =
    //     glm::perspective(glm::radians(mFovCamera), width / height, mNearPlane, mFarPlane);
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
    glm::vec3 direction;
    direction.x = cos(glm::radians(mRotation.yaw)) * cos(glm::radians(mRotation.pitch));
    direction.y = sin(glm::radians(mRotation.pitch));
    direction.z = sin(glm::radians(mRotation.yaw)) * cos(glm::radians(mRotation.pitch));
    mCameraFront = glm::normalize(direction);

    direction.x = cos(glm::radians(mRotation.yaw));
    direction.y = 0;
    direction.z = sin(glm::radians(mRotation.yaw));
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

float Camera::nearPlane() const
{
    return mNearPlane;
}

float Camera::farPlane() const
{
    return mFarPlane;
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