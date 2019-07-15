#pragma once

#include <Urho3D/Scene/LogicComponent.h>

class InteractionCollider : public Urho3D::LogicComponent {
    URHO3D_OBJECT(InteractionCollider, Urho3D::LogicComponent);

public:
    InteractionCollider(Urho3D::Context*);

    void Start() override;

    void handle_collision(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
};
