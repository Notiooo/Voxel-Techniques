#pragma once

#include "World/Polygons/Chunks/Types/PolygonChunk.h"
#include "World/Polygons/Meshes/Model3D.h"

namespace Voxino::Polygons
{

/**
 * It is a large object consisting of a multitude of individual blocks contained within it.
 */
template<typename MeshBuilder>
class ChunkArray : public PolygonChunk
{
public:
    using PolygonChunk::PolygonChunk;


    /**
     * Returns the number of chunk vertices
     * @return Number of vertices
     */
    int numberOfVertices() override;

    /**
     * Returns the size in memory that the mesh occupies
     * @return The size in memory in bytes that the chunk occupies
     */
    int memorySize() override;

    /**
     * \brief Swaps the current chunk mesh with the latest, most recently generated one
     */
    void updateMesh() override;

    /**
     * It is rebuilding this mesh fresh. Very expensive operation
     */
    void rebuildMesh() override;

protected:
    MeshBuilder mTerrainMeshBuilder;
    MeshBuilder mFluidMeshBuilder;
    MeshBuilder mFloralMeshBuilder;
};

template<typename MeshBuilder>
int ChunkArray<MeshBuilder>::numberOfVertices()
{
    return mTerrainModel->mesh().numberOfVertices() + mFloralModel->mesh().numberOfVertices() +
           mFluidModel->mesh().numberOfVertices();
}

template<typename MeshBuilder>
int ChunkArray<MeshBuilder>::memorySize()
{
    return mTerrainModel->mesh().memorySize() + mFloralModel->mesh().memorySize() +
           mFluidModel->mesh().memorySize();
}

template<typename MeshBuilder>
void ChunkArray<MeshBuilder>::rebuildMesh()
{
    MEASURE_SCOPE;
    mTerrainMeshBuilder.resetMesh();
    mFluidMeshBuilder.resetMesh();
    mFloralMeshBuilder.resetMesh();
    prepareMesh();
}

template<typename MeshBuilder>
void ChunkArray<MeshBuilder>::updateMesh()
{
    MEASURE_SCOPE;
    if (!mTerrainModel)
    {
        mTerrainModel = std::make_unique<MeshBuilder::ModelType>();
    }
    mTerrainModel->setMesh(mTerrainMeshBuilder.mesh3D());

    if (!mFluidModel)
    {
        mFluidModel = std::make_unique<MeshBuilder::ModelType>();
    }
    mFluidModel->setMesh(mFluidMeshBuilder.mesh3D());

    if (!mFloralModel)
    {
        mFloralModel = std::make_unique<MeshBuilder::ModelType>();
    }
    mFloralModel->setMesh(mFloralMeshBuilder.mesh3D());
}

}// namespace Voxino::Polygons