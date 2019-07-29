#pragma once

#include "Items/Pickable.hpp"

#include <Urho3D/Scene/LogicComponent.h>

#include <vector>

class Lootable : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Lootable, LogicComponent);

public:
    explicit Lootable(Urho3D::Context* context);

    void add_item(Urho3D::SharedPtr<Pickable> item);
    const std::vector<Urho3D::SharedPtr<Pickable>>& get_items();
    void remove_item(Urho3D::SharedPtr<Pickable> item);

private:
    std::vector<Urho3D::SharedPtr<Pickable>> m_items{};
};
