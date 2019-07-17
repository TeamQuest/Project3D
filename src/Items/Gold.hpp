#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Scene/LogicComponent.h>
#include "Items/Pickable.hpp"

#pragma clang diagnostic pop

class Gold : public Pickable {
    URHO3D_OBJECT(Gold, Urho3D::LogicComponent);

public:
    Gold(Urho3D::Context* context);

    void set_amount(int amout);
    const int& get_amount() const;

private:
    Urho3D::String m_name;
    Urho3D::String m_description;
    int m_amount;
};