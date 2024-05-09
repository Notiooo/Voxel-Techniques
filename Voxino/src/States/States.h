#pragma once

namespace Voxino
{

/**
 * \brief Identifiers of individual states that can be used.
 */
enum class State_ID
{
    // === Application flow states === //
    None,
    PolygonSingleChunkCullingState,
    PolygonSingleChunkNaiveState,
    PolygonSingleChunkGreedyState,
    PolygonSingleChunkBinaryGreedyState,
    PolygonSingleChunkCullingGpuState,
    PolygonMultiChunkCullingState,
    PolygonMultiChunkNaiveState,
    PolygonMultiChunkGreedyState,
    PolygonMultiChunkBinaryGreedyState,
    PolygonMultiChunkCullingGpuState,
    RaycastSingleChunkTexturedVoxels,
    RaycastSingleChunkTexturedVoxelsFixedStep,
    RaycastSingleChunkTexturedBrickmapGpu,
    RaycastSingleChunkTexturedOctreeGpu,
    RaycastMultiChunkTexturedVoxels,
    RaycastMultiChunkTexturedVoxelsFixedStep,
    RaycastMultiChunkTexturedBrickmapGpu,
    RaycastMultiChunkTexturedBrickmap,
    RaycastMultiChunkTexturedOctreeGpu,
    PauseState,

    ExitApplicationState,
};

/**
 * \brief Converts state identifier to text
 * \param stateId State identifier
 * \return Textual representation of the state
 */
inline std::string toString(State_ID stateId)
{
    switch (stateId)
    {

        case State_ID::None: return "None";
        case State_ID::PolygonSingleChunkCullingState: return "PolygonSingleChunkCullingState";
        case State_ID::PolygonSingleChunkNaiveState: return "PolygonSingleChunkNaiveState";
        case State_ID::PauseState: return "PauseState";
        case State_ID::ExitApplicationState: return "ExitApplicationState";
        case State_ID::PolygonSingleChunkGreedyState: return "PolygonSingleChunkGreedyState";
        case State_ID::PolygonSingleChunkCullingGpuState:
            return "PolygonSingleChunkCullingGpuState";
        case State_ID::PolygonSingleChunkBinaryGreedyState:
            return "PolygonSingleChunkBinaryGreedyState";
        case State_ID::PolygonMultiChunkCullingState: return "PolygonMultiChunkCullingState";
        case State_ID::PolygonMultiChunkNaiveState: return "PolygonMultiChunkNaiveState";
        case State_ID::PolygonMultiChunkGreedyState: return "PolygonMultiChunkGreedyState";
        case State_ID::PolygonMultiChunkBinaryGreedyState:
            return "PolygonMultiChunkBinaryGreedyState";
        case State_ID::PolygonMultiChunkCullingGpuState: return "PolygonMultiChunkCullingGpuState";
        case State_ID::RaycastSingleChunkTexturedVoxels: return "RaycastSingleChunkTexturedVoxels";
        case State_ID::RaycastMultiChunkTexturedVoxels: return "RaycastMultiChunkTexturedVoxels";
        case State_ID::RaycastSingleChunkTexturedBrickmapGpu:
            return "RaycastSingleChunkTexturedBrickmapGpu";
        case State_ID::RaycastSingleChunkTexturedOctreeGpu:
            return "RaycastSingleChunkTexturedOctreeGpu";
        default:
            spdlog::warn("I was unable to perform the ID to text conversion.");
            return "Unknown State_ID";
    }
}

}// namespace Voxino
