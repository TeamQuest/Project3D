#pragma once

#include "Character/Character.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>

#pragma clang diagnostic pop

class App : public Urho3D::Application {
public:
    App(Urho3D::Context* context);

    void Setup() override;
    void Start() override;
    void Stop() override;

    void handle_begin_frame(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_key_down(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_key_up(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_update(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_postrender_update(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_closed_pressed(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void handle_post_update(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);

    void init_user_interface();
    void init_scene();
    void create_character();

private:
    Urho3D::SharedPtr<Urho3D::Scene> m_scene = nullptr;
    Urho3D::SharedPtr<Urho3D::Node> m_camera = nullptr;
    Urho3D::WeakPtr<Character> m_character;
};
