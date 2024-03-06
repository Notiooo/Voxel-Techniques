#pragma once

namespace Voxino
{

/**
 * \brief The image, closed in a buffer, lives on the graphics card.
 */
class Texture
{
public:
    /**
     * \brief Type indicates whether it is used for
     * diffuse reflection or sepcular highlight etc.
     */
    enum class Type
    {
        NoSpecified,
        Diffuse,
        Specular
    };

    /**
     * \brief Creates a texture based on the file path to image
     * \param type Texture type
     */
    explicit Texture(Type type = Type::NoSpecified);

    /**
     * \brief Creates a texture based on the file path to image
     * \param filePath Path to the image
     * \param type Texture type
     */
    explicit Texture(const std::string& filePath, Type type = Type::NoSpecified);

    /**
     * \brief Removes texture
     */
    ~Texture();

    /**
     * \brief Copy contructor of texture class
     * \param rhs Other texture to copy
     */
    Texture(const Texture& rhs);

    /**
     * \brief Move constructor of texture class
     * \param rhs Other r-value texture object
     */
    Texture(Texture&& rhs) noexcept;

    /**
     * \brief Move assignment of texture class
     * \param rhs Other r-value texture object
     */
    Texture& operator=(Texture&& rhs) noexcept;

    /**
     * Sets the image under the given path as a texture
     * @param filePath Path to the image
     */
    void loadImage(const std::string& filePath);

    /**
     * Sets the texture smoothing
     * @param isSmooth True if it should be smoothed, false otherwise
     */
    void setSmoothing(bool isSmooth);


    /**
     * \brief Binds a texture for rendering
     * \param slot Slot to which the texture will be binded (can be obtained from the shader level)
     */
    void bind(unsigned int slot = 0) const;

    /**
     * \brief Unbinds the texture from rendering
     */
    void unbind() const;

    /**
     * \brief Returns the width of the texture
     * \return Width of the texture
     */
    [[nodiscard]] int width() const;

    /**
     * \brief Returns the height of the texture
     * \return Height of the texture
     */
    [[nodiscard]] int height() const;

    /**
     * Returns the size of the texture
     * @return Size of the texture in pixels.
     */
    [[nodiscard]] glm::ivec2 size() const;

    /**
     * \brief Returns the aspect ratio of the image
     * \return Aspect ratio of the image
     */
    [[nodiscard]] float aspectRatio() const;

    /**
     * \brief Returns a texture type indicating how to draw it
     */
    [[nodiscard]] Type type() const;

private:
    unsigned int mTextureId;
    std::string mFilePath;
    float mAspectRatio;
    int mWidth, mHeight, mNrChannels;
    Type mTextureType;
};


inline std::string toString(const Texture::Type& textureType)
{
    switch (textureType)
    {
        case Texture::Type::NoSpecified: return "NoSpecified";
        case Texture::Type::Diffuse: return "Diffuse";
        case Texture::Type::Specular: return "Specular";
        default: return "Unknown";
    }
}

}// namespace Voxino