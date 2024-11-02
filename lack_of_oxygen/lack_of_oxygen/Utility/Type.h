#pragma once
#include <cstdint>
#include <bitset>
#include <limits>

// Maximum number of components an entity can have
constexpr std::size_t MAX_COMPONENTS = 64; // Adjust as needed

// Alias for the entity identifier type
using EntityID = std::uint32_t;

// Alias for the signature and component mask
using Signature = std::bitset<MAX_COMPONENTS>;
using ComponentMask = Signature; // Alias for clarity

// Constant representing an invalid entity ID
constexpr EntityID INVALID_ENTITY_ID = std::numeric_limits<EntityID>::max();