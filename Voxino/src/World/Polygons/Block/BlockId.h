#pragma once

namespace Voxino
{

// clang-format off
enum class BlockId
{
    Air          = 0,
    Grass        = 1,
    Dirt         = 2,
    Stone        = 3,
    Water        = 4,
    Sand         = 5,
    Log          = 6,
    Leaves       = 7,
    Cactus       = 8,
    DryGrass     = 9,
    SnowyGrass   = 10,
    SpruceLog    = 11,
    SnowyLeaves  = 12,
    Ice          = 13,
    Planks       = 14,
    SprucePlanks = 15,
    Gravel       = 16,
    Cobblestone  = 17,

    Counter,
    AllBlocks
};
// clang-format on

inline std::string toString(BlockId blockId)
{
    switch (blockId)
    {
        case BlockId::Air: return "Air";
        case BlockId::Grass: return "Grass";
        case BlockId::Dirt: return "Dirt";
        case BlockId::Stone: return "Stone";
        case BlockId::Water: return "Water";
        case BlockId::Sand: return "Sand";
        case BlockId::Log: return "Log";
        case BlockId::Leaves: return "Leaves";
        case BlockId::Cactus: return "Cactus";
        case BlockId::DryGrass: return "DryGrass";
        case BlockId::SnowyGrass: return "SnowyGrass";
        case BlockId::SpruceLog: return "SpruceLog";
        case BlockId::SnowyLeaves: return "SnowyLeaves";
        case BlockId::Ice: return "Ice";
        case BlockId::Planks: return "Planks";
        case BlockId::SprucePlanks: return "SprucePlanks";
        case BlockId::Gravel: return "Gravel";
        case BlockId::Cobblestone: return "Cobblestone";
        case BlockId::Counter:// Fall through to next case
        case BlockId::AllBlocks: return "Special Enum Value";
        default: return "Unknown";
    }
}

}// namespace Voxino