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
    LogoState,
    GameState,
    PolygonSingleChunkCullingState,
    PolygonSingleChunkNaiveState,
    PolygonSingleChunkGreedyState,
    PolygonSingleChunkBinaryGreedyState,
    PolygonSingleChunkCullingGpuState,
    RaycastSingleChunkColoredVoxels,
    RaycastSingleChunkTexturedVoxels,
    RaycastSingleChunkTexturedBrickmapGpu,
    PauseState,

    ExitApplicationState
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
        case State_ID::LogoState: return "LogoState";
        case State_ID::GameState: return "GameState";
        case State_ID::PolygonSingleChunkCullingState: return "PolygonSingleChunkCullingState";
        case State_ID::PolygonSingleChunkNaiveState: return "PolygonSingleChunkNaiveState";
        case State_ID::PauseState: return "PauseState";
        case State_ID::ExitApplicationState: return "ExitApplicationState";
        case State_ID::PolygonSingleChunkGreedyState: return "PolygonSingleChunkGreedyState";
        case State_ID::PolygonSingleChunkCullingGpuState:
            return "PolygonSingleChunkCullingGpuState";
        case State_ID::RaycastSingleChunkColoredVoxels: return "RaycastSingleChunkColoredVoxels";
        default:
            spdlog::warn("I was unable to perform the ID to text conversion.");
            return "Unknown State_ID";
    }
}

}// namespace Voxino
