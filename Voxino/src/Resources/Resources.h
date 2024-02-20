#pragma once

#include "Resources/ResourceManager.h"
#include <Renderer/Graphics/Font.h>
#include <Renderer/Graphics/Texture.h>

namespace Voxino
{

class Player;

/**
 * \brief Texture IDs under which individual textures can be saved and read.
 */
enum class TextureManagerId
{
    // Empty
};

/**
 * \brief Object storing textures of the application
 */
using TextureManager = ResourceManager<Texture, TextureManagerId>;

// ====== Fonts ======= //

/**
 * \brief Fonts IDs under which individual font can be saved and read.
 */
enum class FontId
{
    ArialNarrow,
};

/**
 * \brief Object storing fonts of the application
 */
using FontManager = ResourceManager<Font, FontId>;

/**
 * @brief Any application assets from textures or fonts
 */
struct ApplicationResources
{
    TextureManager textureManager;
    FontManager fontManager;
};

}// namespace Voxino