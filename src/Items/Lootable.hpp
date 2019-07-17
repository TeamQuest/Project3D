#pragma once

#include "Items/Pickable.hpp"

#include <Urho3D/Scene/LogicComponent.h>

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
