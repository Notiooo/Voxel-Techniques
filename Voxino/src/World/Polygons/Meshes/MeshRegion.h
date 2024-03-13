#pragma once

namespace Voxino
{

struct MeshRegion
{
    Block::TextureId id{};
    Block::Face face{};
    Block::Coordinate blockPosition{0, 0, 0};
    std::vector<glm::vec2> textureCoordinates{};
    unsigned width{0};
    unsigned height{0};
};

};// namespace Voxino