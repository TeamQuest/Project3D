#pragma once

#include "Scenes/State.hpp"

class Options : public State {
public:
    Options(Urho3D::Context* context);
    ~Options();

    void Update(float time_step) override;
};
