#pragma once

#include <type_traits>

namespace MovementKey {
// clang-format off
enum MovementKey {
    RESET   = 0u,
    FORWARD = 1u << 1u,
    BACK    = 1u << 2u,
    LEFT    = 1u << 3u,
    RIGHT   = 1u << 4u,
    JUMP    = 1u << 5u,
};
// clang-format on

template <typename T = std::underlying_type_t<MovementKey>>
inline auto operator|(MovementKey lhs, MovementKey rhs)
{
    return static_cast<T>(lhs) | static_cast<T>(rhs);
}
}  // namespace MovementKey

constexpr auto MOVE_FORCE = 1.0f;
constexpr auto INAIR_MOVE_FORCE = 0.08f;
constexpr auto BRAKE_FORCE = 0.2f;
constexpr auto JUMP_FORCE = 5.0f;
constexpr auto YAW_SENSITIVITY = 0.3f;
constexpr auto PITCH_SENSITIVITY = 0.1f;
constexpr auto INAIR_THRESHOLD_TIME = 0.1f;

constexpr auto CAMERA_MIN_DIST = 0.3f;
constexpr auto CAMERA_INITIAL_DIST = 5.0f;
constexpr auto CAMERA_MAX_DIST = 20.0f;
constexpr auto PLAYER_NAME = "Player";
