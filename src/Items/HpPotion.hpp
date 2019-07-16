#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Scene/LogicComponent.h>
#include "Items/Pickable.hpp"

#pragma clang diagnostic pop

class HpPotion : public Pickable {
    URHO3D_OBJECT(HpPotion, Urho3D::LogicComponent);

public:
    HpPotion(Urho3D::Context* context);
};
