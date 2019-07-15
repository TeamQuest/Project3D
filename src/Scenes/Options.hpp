#pragma once

#include "Scenes/State.hpp"

class Options : public State {
public:
    Options(Urho3D::Context* context);
    ~Options();

    void Update(float time_step) override;

    void handle_key_down(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
};
