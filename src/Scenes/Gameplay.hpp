#pragma once

#include "Character/Character.hpp"
#include "Scenes/State.hpp"

class Gameplay : public State {
public:
    explicit Gameplay(Urho3D::Context* context);
    ~Gameplay();

    void init_ui();
    void init_gamescene();
    void handle_key_down(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void update(float time_step) override;

private:
    Urho3D::WeakPtr<Character> m_character;
    Urho3D::WeakPtr<Urho3D::Node> m_camera;
};
