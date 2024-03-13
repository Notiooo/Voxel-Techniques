#include "ChunkMeshBuilder.h"
namespace Voxino
{

float ChunkMeshBuilder::addBlockFaceFakeLightning(const Block::Face& blockFace) const
{
    switch (blockFace)
    {
        case Block::Face::Top: return 1.0f; break;
        case Block::Face::Left: return 0.65f; break;
        case Block::Face::Right: return 0.65f; break;
        case Block::Face::Bottom: return 0.5f; break;
        case Block::Face::Front: return 0.8f; break;
        case Block::Face::Back: return 0.5f; break;
        case Block::Face::Counter: break;
    }
    spdlog::error("Unknown block face, provided enumid: {}. Using maximum lightning.",
                  static_cast<int>(blockFace));
    return 1.0f;
}

void ChunkMeshBuilder::addBlockFaceIndices(std::vector<GLuint>& indices)
{
    // clang-format off
    indices.insert(indices.end(),
                   {
                    mIndex,
                    mIndex + 1,
                    mIndex + 2,

                    mIndex + 2,
                    mIndex + 3,
                    mIndex
                   });
    // clang-format on
    mIndex += 4;
}

std::vector<GLfloat> ChunkMeshBuilder::faceVertices(const Block::Face& blockFace) const
{
    switch (blockFace)
    {
        case Block::Face::Top:
            return {
                // x  y  z
                0, 1, 1,// top far left
                1, 1, 1,// top far right
                1, 1, 0,// top close right
                0, 1, 0,// top close left
            };

        case Block::Face::Left:
            return {
                // x  y  z
                0, 0, 0,// left bottom close
                0, 0, 1,// left bottom far
                0, 1, 1,// left top far
                0, 1, 0 // left top close
            };

        case Block::Face::Right:
            return {
                // x  y  z
                1, 0, 1,// right bottom far
                1, 0, 0,// right bottom close
                1, 1, 0,// right top close
                1, 1, 1 // right top far
            };

        case Block::Face::Bottom:
            return {
                // x  y  z
                0, 0, 0,// bottom left close
                1, 0, 0,// bottom right close
                1, 0, 1,// bottom right far
                0, 0, 1 // bottom left far
            };

        case Block::Face::Front:
            return {
                // x  y  z
                0, 0, 1,// front left bottom
                1, 0, 1,// front right bottom
                1, 1, 1,// front right top
                0, 1, 1 // front left top
            };

        case Block::Face::Back:
            return {
                // x  y  z
                1, 0, 0,// back right bottom
                0, 0, 0,// back left bottom
                0, 1, 0,// back left top
                1, 1, 0,// back right top
            };
        default: throw std::runtime_error("Unsupported Block::Face value was provided");
    }
}

}// namespace Voxino