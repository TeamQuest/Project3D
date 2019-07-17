#pragma once

#include "Items/Pickable.hpp"

#include <Urho3D/Scene/LogicComponent.h>

class HpPotion : public Pickable {
    URHO3D_OBJECT(HpPotion, Urho3D::LogicComponent);

public:
    HpPotion(Urho3D::Context* context);
};
