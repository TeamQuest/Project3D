#pragma once

#include "Scenes/State.hpp"

class Gameplay : public State {
public:
    Gameplay(Urho3D::Context* context);
    ~Gameplay();

    void Update(float time_step) override;
};
