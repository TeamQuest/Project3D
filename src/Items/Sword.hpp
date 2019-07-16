#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Scene/LogicComponent.h>
#include "Items/Pickable.hpp"

#pragma clang diagnostic pop

class Sword : public Pickable {
    URHO3D_OBJECT(Sword, Urho3D::LogicComponent);

public:
    Sword(Urho3D::Context* context);

    void set_dmg(int dmg);
    const int& get_dmg() const;

private:
    int m_dmg;
};