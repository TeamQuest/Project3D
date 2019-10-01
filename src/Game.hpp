#pragma once

#include "Scenes/Scenes.hpp"
#include "Scenes/State.hpp"

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>

class Game : public Urho3D::Application {
public:
    explicit Game(Urho3D::Context* context);

    void Setup() override;
    void Start() override;
    void Stop() override;

    void handle_change_state(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_key_down(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_update(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_postrender_update(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_post_update(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);

    void init_user_interface();
    void init_scene();

private:
    Urho3D::SharedPtr<Urho3D::Node> m_camera = nullptr;
    Urho3D::UniquePtr<State> m_active_state;
    Scenes m_next_state = Scenes::Empty;
};
