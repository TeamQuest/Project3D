#pragma once

#include <type_traits>

namespace MovementKey {
// clang-format off
enum MovementKey {
    RESET   = 0,
    FORWARD = 1 << 1,
    BACK    = 1 << 2,
    LEFT    = 1 << 3,
    RIGHT   = 1 << 4,
    JUMP    = 1 << 5,
    PUNCH   = 1 << 6,
    SWIPE   = 1 << 7,
    KICK    = 1 << 8,
};
// clang-format on

template <typename T = std::underlying_type_t<MovementKey>>
inline auto operator|(MovementKey lhs, MovementKey rhs)
{
    return static_cast<T>(lhs) | static_cast<T>(rhs);
}
}  // namespace MovementKey

constexpr auto MOVE_FORCE = 0.8f;
constexpr auto INAIR_MOVE_FORCE = 0.08f;
constexpr auto BRAKE_FORCE = 0.2f;
constexpr auto JUMP_FORCE = 7.0f;
constexpr auto YAW_SENSITIVITY = 0.3f;
constexpr auto PITCH_SENSITIVITY = 0.1f;
constexpr auto INAIR_THRESHOLD_TIME = 0.1f;

constexpr auto CAMERA_MIN_DIST = 1.0f;
constexpr auto CAMERA_INITIAL_DIST = 5.0f;
constexpr auto CAMERA_MAX_DIST = 20.0f;
