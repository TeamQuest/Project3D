#pragma once

#include "Items/Pickable.hpp"

#include <Urho3D/Scene/LogicComponent.h>

class Gold : public Pickable {
    URHO3D_OBJECT(Gold, Urho3D::LogicComponent);

public:
    explicit Gold(Urho3D::Context* context);

    void set_amount(int value);
    const int& get_amount() const;

private:
    int m_amount;
};
