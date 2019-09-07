#pragma once

#include <Urho3D/Scene/LogicComponent.h>

class Npc : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Npc, LogicComponent);

public:
    explicit Npc(Urho3D::Context* context);

    void Start() override;
    void Update(float time_step) override;

    void stop_walking();
    void resume();
    bool focused();

    void follow(Urho3D::Node* target);

private:
    void handle_collision(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void correct_speed();

private:
    float move_speed;
    Urho3D::Quaternion saved_rotation;
    Urho3D::Node* target = nullptr;
};
