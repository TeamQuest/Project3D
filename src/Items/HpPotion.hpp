#pragma once

#include "Items/Pickable.hpp"

#include <Urho3D/Scene/LogicComponent.h>

class HpPotion : public Pickable {
    URHO3D_OBJECT(HpPotion, Urho3D::LogicComponent);

public:
    explicit HpPotion(Urho3D::Context* context);
    const int& get_hp_points_to_restore() const;

private:
    int m_hp_points = 100;
};
