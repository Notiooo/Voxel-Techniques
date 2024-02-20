#pragma once

namespace Voxino
{

/**
 * \brief Structure representing one vertex in 3d space
 */
struct Vertex
{
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 textureCoordinates{};
};

}// namespace Voxino