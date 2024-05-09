#include "PolygonChunk.h"
#include "Resources/TexturePackArray.h"
#include "World/Polygons/Meshes/Model3D.h"
#include "pch.h"

namespace Voxino::Polygons
{

void PolygonChunk::draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(mChunkPosition.x, mChunkPosition.y, mChunkPosition.z));
    shader.bind();
    shader.setUniform("model", model);
    drawTerrain(renderer, shader, camera);
}

void PolygonChunk::drawTerrain(const Renderer& renderer, const Shader& shader,
                               const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    mTexturePack.bind(TexturePack::Spritesheet::Blocks);
    if (mTerrainModel)
    {
        mTerrainModel->draw(renderer, shader, camera);
    }
}

}// namespace Voxino::Polygons