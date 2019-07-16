#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Scene/LogicComponent.h>
#include "Items/Pickable.hpp"

#pragma clang diagnostic pop

class PickableSword : public Pickable {
    URHO3D_OBJECT(PickableSword, Urho3D::LogicComponent);

public:
    PickableSword(Urho3D::Context* context);

    void set_name(const Urho3D::String& item);
    const Urho3D::String& get_name() const;

    void set_description(const Urho3D::String& item);
    const Urho3D::String& get_desctiption() const;

    void set_dmg(int dmg);
    const int& get_dmg() const;

private:
    Urho3D::String m_name;
    Urho3D::String m_description;
    int m_dmg;
};