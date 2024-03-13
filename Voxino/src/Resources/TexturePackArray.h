#pragma once
#include "Resources/TexturePack.h"

namespace Voxino
{
/**
 * A class that holds the game's textures.
 * Allows to easily read them, or change the file it read from.
 */
class TexturePackArray : public TexturePack
{
public:
    TexturePackArray() = default;
    TexturePackArray(const std::string& TexturePackArrayFolder);
    /**
     * Allows to load any texture pack from the resources folder
     * @param texturePackFolder Name of the folder with the texture pack
     */
    void loadTexturePack(const std::string& texturePackFolder) override;

    /**
     * Bind a texture pack for rendering
     */
    void bind(const Spritesheet& textures) const override;


private:
    int mTextureSize;
    std::string mTexturePackName;
    unsigned int mTextureId;
    std::string mFilePath;
    float mAspectRatio;
    int mWidth, mHeight, mNrChannels;
};
}// namespace Voxino