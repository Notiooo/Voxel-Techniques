#include "TexturePackAtlas.h"
#include "pch.h"

namespace Voxino
{

TexturePackAtlas::TexturePackAtlas(const std::string& texturePackFolder)
{
    loadTexturePack(texturePackFolder);
}

void TexturePackAtlas::loadTexturePack(const std::string& texturePackFolder)
{
    mTexturePackName = texturePackFolder;
    mTextureSize = 16;// TODO: It should be read from texturepack config?
    mBlocks.loadImage("resources/textures/texturePacks/" + texturePackFolder + "/blocks.png");
    mBlocks.setSmoothing(false);

    if ((mBlocks.width() % mTextureSize) && (mBlocks.height() % mTextureSize))
    {
        throw std::logic_error("The texture pack: " + mTexturePackName +
                               "- have improper dimensions of blocks!");
    }
}

void TexturePackAtlas::bind(const Spritesheet& textures) const
{
    switch (textures)
    {
        case Spritesheet::Blocks:
            mBlocks.bind();
            break;
            // case Spritesheet::Items: sf::Texture::bind(&mItems); break;
    }
}

std::vector<glm::vec2> TexturePackAtlas::normalizedCoordinates(Block::TextureId textureId) const
{
    const auto blocksPerRow = mBlocks.width() / mTextureSize;
    const auto sizeOfPixel = 1.0f / static_cast<float>(mBlocks.height());
    const auto blockRow = textureId / blocksPerRow;
    const auto blockCol = textureId % blocksPerRow;

    auto left = blockCol * mTextureSize * sizeOfPixel;
    auto right = (blockCol + 1) * mTextureSize * sizeOfPixel;
    auto bottom = 1.0f - blockRow * mTextureSize * sizeOfPixel;
    auto top = 1.0f - (blockRow + 1) * mTextureSize * sizeOfPixel;

    // clang-format off
    return
    {
        glm::vec2(right, top),
        glm::vec2(left, top),
        glm::vec2(left, bottom),
        glm::vec2(right, bottom)
    };
    // clang-format on
}

const Texture& TexturePackAtlas::texture(const TexturePack::Spritesheet& textures) const
{
    switch (textures)
    {
        case Spritesheet::Blocks: return mBlocks;
        case Spritesheet::Items:
            spdlog::error("Items spritesheet in texture pack is not supported yet!");
            break;
        default:
            spdlog::error("Unknown SpriteSheet type in Texture Pack, enumid: {}",
                          static_cast<int>(textures));
    }
    // TODO:  Maybe should use Result functionality later on?
    spdlog::error("Returning SpriteSheet of blocks at this moment to handle the error.");
    return mBlocks;
}

sf::Vector2i TexturePackAtlas::pixelCoordinates(unsigned int textureId) const
{
    // assert(mBlocks.size() == mItems.size());
    const auto texturesPerRow = mBlocks.width() / mTextureSize;

    return {static_cast<int>((textureId % texturesPerRow) * mTextureSize),
            static_cast<int>((((textureId / texturesPerRow) * mTextureSize)))};
}

}// namespace Voxino