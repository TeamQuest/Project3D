#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Scene/LogicComponent.h>

#pragma clang diagnostic pop

class Npc : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Npc, LogicComponent);

public:
    explicit Npc(Urho3D::Context* context);

    void Start() override;
    void Update(float time_step) override;

    void set_focused(Urho3D::Node* node);

private:
    void handle_collision(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);

private:
};
