#pragma once

#include "Items/Pickable.hpp"

#include <Urho3D/Scene/LogicComponent.h>

class Sword : public Pickable {
    URHO3D_OBJECT(Sword, Urho3D::LogicComponent);

public:
    explicit Sword(Urho3D::Context* context);

    void set_dmg(int value);
    int get_dmg() const;

private:
    int m_dmg;
};
