#pragma once

#include <Urho3D/Scene/LogicComponent.h>

class FPSCounter final : public Urho3D::LogicComponent {
    URHO3D_OBJECT(FPSCounter, LogicComponent);

public:
    explicit FPSCounter(Urho3D::Context*);
    ~FPSCounter();

    void Update(float time_step) override;

private:
    float m_counter = 0.f;
    const float FPS_UPDATE_TIME = 0.5f;  // in seconds
};
