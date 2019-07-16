#pragma once

#include "Items/Pickable.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"
#include <Urho3D/Scene/LogicComponent.h>
#pragma clang diagnostic pop

#include <vector>

class Lootable : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Lootable, LogicComponent);

public:
    Lootable(Urho3D::Context* context);

    void add_item(Pickable* item);
    const std::vector<Pickable*>& get_items();
    void remove_item(Pickable* item);

private:
    std::vector<Pickable*> m_items{};
};