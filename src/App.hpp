#pragma once

// #include <Urho3D/Engine/Engine.h>
// #include <Urho3D/Graphics/Model.h>

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>

class App : public Urho3D::Application {
public:
    App(Urho3D::Context* context);

    void Setup() override;
    void Start() override;
    void Stop() override;

    void handle_begin_frame(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void handle_key_down(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void handle_update(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void handle_closed_pressed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    int m_framecount = 0;
    float m_time = 0.f;
    Urho3D::SharedPtr<Urho3D::Scene> m_scene = nullptr;
    Urho3D::SharedPtr<Urho3D::Node> m_camera = nullptr;
};
