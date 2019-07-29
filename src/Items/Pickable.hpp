#pragma once

#include <Urho3D/Scene/LogicComponent.h>

class Pickable : public Urho3D::Object {
    URHO3D_OBJECT(Pickable, Urho3D::Object);

public:
    Pickable(Urho3D::Context* context);

    void set_name(const Urho3D::String& name);
    const Urho3D::String& get_name() const;

    void set_description(const Urho3D::String& description);
    const Urho3D::String& get_description() const;

private:
    Urho3D::String m_name;
    Urho3D::String m_description;
};
