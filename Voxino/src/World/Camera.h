#pragma once
#include "Application.h"

#include <Renderer/Graphics/3D/Utils/Rotation3D.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace Voxino
{
/**
 * An in-application camera through which the user views gameplay.
 */
class Camera
{
public:
    explicit Camera(const WindowToRender& target);

    /**
     * \brief Updates the camera logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     */
    void fixedUpdate(const float& deltaTime);

    /**
     * Updates the camera logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the application was last updated.
     */
    void update(const float& deltaTime);

    /**
     * \brief It takes input (event) from the user and interprets it.
     * \param event user input
     */
    void handleEvent(const sf::Event& event);

    /**
     * Returns the view matrix.
     * @return View matrix.
     */
    glm::mat4 view() const;

    /**
     * Returns projection matrix.
     * @return Projection matrix.
     */
    glm::mat4 projection() const;

    /**
     * Returns the current position of the camera.
     * @return Current position of the camera.
     */
    glm::vec3 cameraPosition() const;

    /**
     * @brief Sets the camera position
     * @param newPosition The new position on which the camera should be placed.
     */
    void cameraPosition(const glm::vec3& newPosition);

    /**
     * Camera viewing direction (front).
     * @return Returns front camera viewing direction.
     */
    glm::vec3 direction() const;

    /**
     * Returns the direction of the camera (front) without taking into account the pitch, i.e. the
     * up-down rotation of the camera.
     * @return Returns front camera viewing direction without pitch (no up-down rotation).
     */
    glm::vec3 directionWithoutPitch() const;

    /**
     * @brief Returns the right direction of the camera, which is the cross product of the front and
     * upward vectors.
     * @return Right direction of the camera.
     */
    glm::vec3 rightDirection() const;

    /**
     * @brief Returns the right direction of the camera without pitch, which is the cross product of
     * the front and upward vectors without respecting pitch, that is, the up-and-down rotation of
     * the camera.
     * @return Right direction of the camera without pitch (without up-and-down rotation of camera)
     */
    glm::vec3 rightDirectionWithoutPitch() const;

    /**
     * @brief Returns the upward direction of the camera
     * @return Upward direction of the camera.
     */
    glm::vec3 upwardDirection() const;

    /**
     * @brief Activates/deactivates in-application controls
     */
    void toggleControls();

    /**
     * \brief Updates the status/logic of the ImGui Debug Menu
     */
    void updateImGui();

    /**
     * \brief Retrieves the current rotation of the object.
     * \return The object's rotation as a Rotation3D instance.
     */
    Rotation3D rotation() const;

    /**
     * \brief Shakes the camera
     */
    void shake();

private:
    /**
     * Handle keyboard behavior such as moving the camera inside the application
     * @param deltaTime the time that has passed since the application was last updated.
     */
    void handleKeyboardInputs(const float& deltaTime);

    /**
     * It changes speed depending on whether the user has pressed
     * the button responsible for accelerating the camera.
     *
     * @param cameraSpeed Base camera speed
     * @return Accelerated or base camera speed
     */
    static float applyAdditionalCameraAcceleration(float cameraSpeed);

    /**
     * Handles keyboard behavior for the camera, such as moving the camera.
     * @param deltaTime the time that has passed since the application was last updated.
     * @param cameraSpeed Base camera speed
     */
    void handleCameraMovement(const float& deltaTime, float cameraSpeed);

    /**
     * Handles mouse behavior for the camera, such as rotating the camera.
     * @param deltaTime the time that has passed since the application was last updated.
     */
    void handleMouseInputs(const float& deltaTime);

    /**
     * It keeps an eye on the intervals for pitch, so that moving the camera is natural
     * and takes place within the intervals known by the users. For example, it is
     * impossible to perform a 360-degree backflip.
     */
    void keepNaturalPitchRanges();

    /**
     * Calculates yaw and pitch for the camera
     * @param deltaTime the time that has passed since the application was last updated.
     */
    void calculateCameraAngles(const float& deltaTime);

    /**
     * Calculates the camera's directional vector, which is the final
     * vector responsible for the camera's rotation.
     */
    void calculateCameraDirectionVector();

    /**
     * \brief Updates the values responsible for shaking the camera
     * \param deltaTime the time that has passed since the application was last updated.
     */
    void updateShakeValues(const float& deltaTime);

private:
    const WindowToRender& mRenderTarget;
    glm::vec3 mCameraPosition = glm::vec3(0, 0, 0);
    glm::vec3 mCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 mCameraFrontWithoutPitch = glm::vec3(0.0f, 0.0f, -1.0f);
    Rotation3D mRotation{-90, 0, 0.f};

    Rotation3D mTargetShakeValues{0, 0, 0};
    Rotation3D mCurrentShakeValues{0, 0, 0};

    float mCameraSpeed = 5.f;
    float mCameraSensitivity = 4.f;
    float mFovCamera = 90.0f;
    bool mAreControlsEnabled = true;

    glm::mat4 mViewMatrix{};
    mutable glm::mat4 mProjectionMatrix{};
};
}// namespace Voxino