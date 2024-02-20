#pragma once
#include <Renderer/Graphics/2D/Sprite2D.h>
#include <World/Camera.h>

namespace Voxino
{
class Renderer;
}

namespace sf
{
class Event;
}

namespace Voxino
{
class Camera;

/**
 * \brief The class that defines the user (physics, camera and all things related to the user)
 */
class Player
{
public:
    /**
     * \brief Player class constructor
     * \param window The window into which the player's eye view is rendered
     * \param colliderRegister Register in which all collisions on the scene should be located
     */
    explicit Player(WindowToRender& window);

    static constexpr auto PLAYER_MAX_FLYING_SPEED = 5.f;
    static constexpr auto PLAYER_FLYING_DECELERATE_RATIO = 10.f;
    static constexpr auto PLAYER_ACCELERATE_SPEED = 2.5f;

    /**
     * \brief Draws all player components to a given target
     * \param target The target to which the model is drawn
     */
    void draw(const Renderer& target) const;

    /**
     * Updates the Player logic dependent, or independent of time, every rendered frame.
     * \param deltaTime the time that has passed since the engine was last updated.
     */
    void update(const float& deltaTime);

    /**
     * \brief Updates the Player logic at equal intervals independent of the frame rate.
     * \param deltaTime Time interval
     */
    void fixedUpdate(const float& deltaTime);

    /**
     * @brief Handles keyboard states such as button presses and responds accordingly
     * @param deltaTime the time that has passed since the engine was last updated.
     */
    void handleMovementKeyboardInputs(const float& deltaTime);

    /**
     * \brief It takes input (event) from the user and interprets it
     * \param event user input
     */
    void handleEvent(const sf::Event& event);

    /**
     * @brief Enables/disables player controls.
     */
    void toggleControls();

    /**
     * \brief Returns the player's camera
     * \return Player's camera
     */
    Camera& camera();

    /**
     * \brief Returns the player's camera
     * \return Player's camera
     */
    [[nodiscard]] const Camera& camera() const;

private:
    /**
     * \brief Updates the player's physics
     * \param deltaTime the time that has passed since the engine was last updated.
     */
    void updatePhysics(float deltaTime);

    /**
     * \brief Reduces the player's velocity by gradually decelerating it
     * \param deltaTime the time that has passed since the engine was last updated.
     */
    void decelerateVelocity(const float& deltaTime);

    /**
     * \brief Limits player's speed to maximum level
     * \param deltaTime the time that has passed since the engine was last updated.
     */
    void limitVelocity(const float& deltaTime);

private:
    Camera mCamera;
    glm::vec3 mPosition{0, 0, 0};
    glm::vec3 mVelocity{0, 0, 0};
    Texture mCrosshairTexture;
    Sprite2D mCrosshair;
    bool doesPlayerStandOnCollider = false;
};
}// namespace Voxino