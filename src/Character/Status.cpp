#include "Status.hpp"

#include "Scenes/Scenes.hpp"

#include <Urho3D/IO/Log.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

Status::Status(Context *context) : LogicComponent(context) {
    URHO3D_LOGINFO("MainMenu scene enabled");
}

void Status::set_hp_points(float points) {
    m_hp_points = points;
}

float Status::get_hp_points() {
    if (m_hp_points <= 0) {
        SendEvent(E_GAMEOVERREQUESTED);
    }
    return m_hp_points;
}

void Status::set_character_name(const String& name) {
    m_name = name;
}

String Status::get_character_name() {
    return m_name;
}
