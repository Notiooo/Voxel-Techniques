#include "BlockMap.h"
#include "pch.h"
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace Voxino
{

const BlockMap& BlockMap::blockMap()
{
    static const BlockMap instance;

    return instance;
}

const BlockType& BlockMap::blockType(const BlockId& blockId) const
{
    return mBlockMap.at(blockId);
}

BlockMap::BlockMap()
{
    parseDirectory("resources/");
}

void BlockMap::parseDirectory(const std::string& directoryName)
{
    auto currentId = 0;
    auto blocks = YAML::LoadFile(directoryName + "blocks.yaml");
    auto textures = YAML::LoadFile(directoryName + "textures.yaml");

    std::map<std::string, int> textureMapping;
    for (auto texture: textures["textures"])
    {
        textureMapping[texture.first.as<std::string>()] = texture.second.as<int>();
    }

    for (auto block: blocks["blocks"])
    {
        BlockType blockType;
        blockType.id = static_cast<BlockId>(currentId++);
        blockType.name = block["name"].as<std::string>();

        auto setIfPresent = [&block, &blockType, &textureMapping](const std::string& setting,
                                                                  const Block::Face& blockFace)
        {
            if (block[setting])
            {
                blockType.textureId[blockFace] =
                    static_cast<Block::TextureId>(textureMapping[block[setting].as<std::string>()]);
            }
        };

        if (block["texture"])
        {
            auto generalTexture =
                static_cast<Block::TextureId>(textureMapping[block["texture"].as<std::string>()]);
            for (auto i = 0; i < static_cast<int>(Block::Face::Counter); ++i)
            {
                blockType.textureId[static_cast<Block::Face>(i)] = generalTexture;
            }
        }

        if (block["textureSide"])
        {
            blockType.textureId[Block::Face::Left] = static_cast<Block::TextureId>(
                textureMapping[block["textureSide"].as<std::string>()]);
            blockType.textureId[Block::Face::Right] = static_cast<Block::TextureId>(
                textureMapping[block["textureSide"].as<std::string>()]);
            blockType.textureId[Block::Face::Front] = static_cast<Block::TextureId>(
                textureMapping[block["textureSide"].as<std::string>()]);
            blockType.textureId[Block::Face::Back] = static_cast<Block::TextureId>(
                textureMapping[block["textureSide"].as<std::string>()]);
        }

        // Set other texture faces of blocks (optional)
        setIfPresent("textureLeft", Block::Face::Left);
        setIfPresent("textureRight", Block::Face::Right);
        setIfPresent("textureTop", Block::Face::Top);
        setIfPresent("textureBottom", Block::Face::Bottom);
        setIfPresent("textureFront", Block::Face::Front);
        setIfPresent("textureBack", Block::Face::Back);

        blockType.transparent = block["transparent"] ? block["transparent"].as<bool>() : false;
        blockType.collidable = block["transparent"] ? block["transparent"].as<bool>() : true;
        mBlockMap[blockType.id] = blockType;
    }
}

}// namespace Voxino