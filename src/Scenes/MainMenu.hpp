#pragma once

#include "Scenes/State.hpp"

class MainMenu : public State {
public:
    explicit MainMenu(Urho3D::Context* context);
    ~MainMenu();

    void update(float time_step) override;
    void handle_key_down(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
};
