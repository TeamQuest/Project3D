#pragma once

#include "Scenes/State.hpp"

class MainMenu : public State {
public:
    MainMenu(Urho3D::Context* context);

    void Update(float time_step) override;

    void handle_closed_pressed(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
};