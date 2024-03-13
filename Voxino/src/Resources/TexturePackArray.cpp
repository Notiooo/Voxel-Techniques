#include "TexturePackArray.h"
#include "pch.h"

#include <Renderer/Core/OpenglUtils.h>

namespace Voxino
{

TexturePackArray::TexturePackArray(const std::string& texturePackFolder)
{
    loadTexturePack(texturePackFolder);
}

void TexturePackArray::loadTexturePack(const std::string& texturePackFolder)
{
    mTexturePackName = texturePackFolder;
    mTextureSize = 16;// TODO: It should be read from texturepack config?

    stbi_set_flip_vertically_on_load(false);
    auto filePath =
        std::string("resources/textures/texturePacks/" + texturePackFolder + "/blocks.png");
    auto mData = stbi_load(filePath.c_str(), &mWidth, &mHeight, &mNrChannels, 4);

    if (!mData)
    {
        spdlog::error("Failed to load texture data from file: {}", filePath);
    }

    GLCall(glGenTextures(1, &mTextureId));
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureId));

    int numSpritesX = mWidth / mTextureSize;
    int numSpritesY = mHeight / mTextureSize;
    int totalSprites = numSpritesX * numSpritesY;

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int tileW = 16;
    int tileH = 16;
    int channels = 4;

    int tilesX = 16;
    int tilesY = 16;
    int imageCount = tilesX * tilesY;

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, tileW, tileH, imageCount, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);

    std::vector<unsigned char> tile(tileW * tileH * channels);
    int tileSizeX = tileW * channels;
    int rowLen = tilesX * tileSizeX;//

    for (int iy = 0; iy < tilesY; ++iy)
    {
        for (int ix = 0; ix < tilesX; ++ix)
        {
            unsigned char* ptr = mData + iy * rowLen * tileH + ix * tileSizeX;
            for (int row = 0; row < tileH; ++row)
            {
                std::copy(ptr + row * rowLen, ptr + row * rowLen + tileSizeX,
                          tile.begin() + row * tileSizeX);
            }


            int i = iy * tilesX + ix;
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tileW, tileH, 1, GL_RGBA,
                            GL_UNSIGNED_BYTE, tile.data());
        }
    }
    stbi_set_flip_vertically_on_load(true);
    stbi_image_free(mData);
}

void TexturePackArray::bind(const Spritesheet& textures) const
{
    switch (textures)
    {
        case Spritesheet::Blocks:
            glActiveTexture(GL_TEXTURE0);
            GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureId));
            break;
            // case Spritesheet::Items: sf::Texture::bind(&mItems); break;
    }
}

}// namespace Voxino