#pragma once

#include "Scenes/State.hpp"

class Options : public State {
public:
    explicit Options(Urho3D::Context* context);
    ~Options();

    void update(float time_step) override;
};
