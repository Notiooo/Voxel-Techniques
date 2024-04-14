#include "PolygonChunk.h"
#include "Resources/TexturePackArray.h"
#include "World/Polygons/Meshes/Model3D.h"
#include "pch.h"

namespace Voxino::Polygons
{

void PolygonChunk::draw(const Renderer& renderer, const Shader& shader, const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    drawTerrain(renderer, shader, camera);
    drawLiquids(renderer, shader, camera);
    drawFlorals(renderer, shader, camera);
    // TODO: This will cause problems when many chunks are drawn,
    // but I need to change this system anyway
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

void PolygonChunk::drawLiquids(const Renderer& renderer, const Shader& shader,
                               const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    mTexturePack.bind(TexturePack::Spritesheet::Blocks);
    if (mFluidModel)
    {
        mFluidModel->draw(renderer, shader, camera);
    }
}

void PolygonChunk::drawFlorals(const Renderer& renderer, const Shader& shader,
                               const Camera& camera) const
{
    MEASURE_SCOPE_WITH_GPU;
    mTexturePack.bind(TexturePack::Spritesheet::Blocks);
    if (mFloralModel)
    {
        mFloralModel->draw(renderer, shader, camera);
    }
}

}// namespace Voxino::Polygons