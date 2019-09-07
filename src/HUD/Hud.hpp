#pragma once

#include <Urho3D/Scene/LogicComponent.h>

namespace Urho3D {
class Window;
}

class Hud : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Hud, LogicComponent);

public:
    Hud(Urho3D::Context* context);

    void Start() override;
    void Update(float time_step) override;

private:
    /* member variables */
};
