#pragma once
#include "Resources/TexturePack.h"

namespace Voxino
{
/**
 * A class that holds the game's textures.
 * Allows to easily read them, or change the file it read from.
 */
class TexturePackAtlas : public TexturePack
{
public:
    TexturePackAtlas() = default;
    TexturePackAtlas(const std::string& texturePackFolder);

    /**
     * Allows to load any texture pack from the resources folder
     * @param texturePackFolder Name of the folder with the texture pack
     */
    void loadTexturePack(const std::string& texturePackFolder) override;

    /**
     * Bind a texture pack for rendering
     */
    void bind(const Spritesheet& textures) const override;

    /**
     * Returns normalized texture coordinates that can be passed directly along with the vertices to
     * draw the texture.
     * @param textureId
     * @return
     */
    std::vector<glm::vec2> normalizedCoordinates(Block::TextureId textureId) const;

    /**
     * @brief Returns texture coordinates given in pixels
     * @param textureId Texture identifier
     * @return 2D texture coordinates in pixels
     */
    sf::Vector2i pixelCoordinates(unsigned int textureId) const;

    /**
     * @brief Returns the spritesheet for a given texture type
     * @param textures Different type of spritesheet, such as blocks or items.
     * @return Texture, which is a spritesheet.
     */
    const Texture& texture(const Spritesheet& textures) const;

private:
    Texture mBlocks;

    int mTextureSize;
    std::string mTexturePackName;
};
}// namespace Voxino