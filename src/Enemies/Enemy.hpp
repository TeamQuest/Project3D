#pragma once

#include "Enemies/Enemy.hpp"

#include <Urho3D/Scene/LogicComponent.h>

#include <map>

namespace Urho3D {
    class Window;
}

class Enemy : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Enemy, LogicComponent);

public:
    explicit Enemy(Urho3D::Context* context);
    ~Enemy() override = default;

    void Start() override;
    void Update(float time_step) override;

    void set_hp_points(float points);
    float get_hp_points();

private:
    float m_hp_points = 53.f;
};
