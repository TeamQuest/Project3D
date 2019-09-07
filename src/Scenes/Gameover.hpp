#pragma once

#include "Scenes/State.hpp"

class Gameover : public State {
public:
    explicit Gameover(Urho3D::Context* context);
    ~Gameover() override = default;

    void update(float time_step) override;

private:
    Urho3D::WeakPtr<Urho3D::Node> m_camera;
};
