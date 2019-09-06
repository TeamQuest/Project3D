#include "Status.hpp"

#include <Urho3D/IO/Log.h>
using namespace Urho3D;

Status::Status(Urho3D::Context *context) : LogicComponent(context) {
    URHO3D_LOGINFO("MainMenu scene enabled");
};

void Status::set_hp_points(float points) {
    hp_points = points;
}

float Status::get_hp_points() {
    return hp_points;
}