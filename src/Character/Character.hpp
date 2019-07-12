#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Scene/LogicComponent.h>

#pragma clang diagnostic pop

class Character : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Character, LogicComponent);

public:
    explicit Character(Urho3D::Context* context);

    void Start() override;
    void FixedUpdate(float time_step) override;
    // static void register_object(Urho3D::Context* context);

    void handle_movement();
    void handle_camera(Urho3D::SharedPtr<Urho3D::Node> camera, Urho3D::PhysicsWorld* world);
    void adjust_head_pitch();

private:
    void handle_collision(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_interaction(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);

private:
    Urho3D::Controls m_controls;
    Urho3D::Node* m_action_collider;
    bool m_on_ground;
    bool m_can_jump;
    float m_time_in_air;
};
