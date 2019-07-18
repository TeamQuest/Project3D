#pragma once

#include <Urho3D/Scene/LogicComponent.h>

namespace Urho3D {
class Window;
}

class InteractionCollider : public Urho3D::LogicComponent {
    URHO3D_OBJECT(InteractionCollider, Urho3D::LogicComponent);

public:
    InteractionCollider(Urho3D::Context*);

    void Start() override;
    void Update(float time_step) override;

    void handle_collision();
    void handle_interaction();
    void close_window();

private:
    Urho3D::Window* create_popup_window();

private:
    Urho3D::WeakPtr<Urho3D::Node> m_highlighted;
    bool m_window_open = false;
};
