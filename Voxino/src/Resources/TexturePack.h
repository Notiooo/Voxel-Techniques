#pragma once
#include "World/Block/Block.h"

namespace Voxino
{
/**
 * A class that holds the game's textures.
 * Allows to easily read them, or change the file it read from.
 */
class TexturePack : public sf::NonCopyable
{
public:
    virtual ~TexturePack() = default;
    static constexpr auto TEXTURE_SIZE = 16;
    enum class Spritesheet
    {
        Blocks,
        Items
    };

    /**
     * Allows to load any texture pack from the resources folder
     * @param texturePackFolder Name of the folder with the texture pack
     */
    virtual void loadTexturePack(const std::string& texturePackFolder) = 0;

    /**
     * Bind a texture pack for rendering
     */
    virtual void bind(const Spritesheet& textures) const = 0;
};
}// namespace Voxino