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
    PolygonSingleChunkState,
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
        case State_ID::PolygonSingleChunkState: return "PolygonSingleChunk";
        case State_ID::PauseState: return "PauseState";
        case State_ID::ExitApplicationState: return "ExitApplicationState";
        default:
            spdlog::warn("I was unable to perform the ID to text conversion.");
            return "Unknown State_ID";
    }
}

}// namespace Voxino
