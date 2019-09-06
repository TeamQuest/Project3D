#pragma once

#include <Urho3D/Scene/LogicComponent.h>

class Status : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Status, Urho3D::LogicComponent);
public:
    explicit Status(Urho3D::Context *context);

    void set_hp_points(float points);
    float get_hp_points();

    void set_character_name(Urho3D::String n);
    Urho3D::String get_character_name();

private:
    float hp_points = 100.f;
    Urho3D::String name = "unknow";
};


