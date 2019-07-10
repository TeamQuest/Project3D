#include "App.hpp"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>

using namespace Urho3D;

App::App(Context* context) : Application{context}, m_scene{new Scene{context}}
{
}

void App::Setup()
{
    engineParameters_["FullScreen"] = false;
    engineParameters_["WindowWidth"] = 1280;
    engineParameters_["WindowHeight"] = 720;
    engineParameters_["WindowResizable"] = false;
    engineParameters_["VSync"] = true;
    engine_->SetMaxInactiveFps(20);
}

void App::Start()
{
    GetSubsystem<Input>()->SetMouseVisible(true);

    const auto cache = GetSubsystem<ResourceCache>();
    m_scene->CreateComponent<DebugRenderer>();
    m_scene->CreateComponent<Octree>();

    init_user_interface();

    /* Ground example */
    {
        auto floor = m_scene->CreateChild("Floor");
        floor->SetPosition(Vector3(0.f, -5.f, 0.f));
        floor->SetScale(Vector3(500.f, 1.f, 500.f));
        auto floor_model = floor->CreateComponent<StaticModel>();
        floor_model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        floor_model->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));
        auto body = floor->CreateComponent<RigidBody>();
        auto collider = floor->CreateComponent<CollisionShape>();
        collider->SetBox(Vector3::ONE);
        auto [x, y, z] = floor->GetPosition();
        // Custom warning
        URHO3D_LOGWARNINGF("Creating ground at position (%f, %f, %f).", x, y, z);
        URHO3D_LOGINFO("Model name: " + floor_model->GetModel()->GetName());
    }
    /* - */

    constexpr auto NUM_OBJECTS = 2000u;
    for (unsigned i = 0; i < NUM_OBJECTS; ++i) {
        auto box = m_scene->CreateChild("Box");
        box->SetPosition(Vector3(Random(200.f) - 100.f, Random(200.f) + 5.f, Random(200.f) - 100.f));
        box->SetRotation(Quaternion(Random(360.f), Random(360.f), Random(360.f)));
        auto box_model = box->CreateComponent<StaticModel>();
        box_model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        box_model->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
    }
    {
        auto sky = m_scene->CreateChild("Sky");
        auto skybox = sky->CreateComponent<Skybox>();
        skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));
    }
    {
        m_camera = m_scene->CreateChild("Camera");
        auto camera = m_camera->CreateComponent<Camera>();
        camera->SetFarClip(2000);
        auto light = m_camera->CreateComponent<Light>();
        light->SetLightType(LIGHT_POINT);
        light->SetRange(200.f);
    }
    {
        auto renderer = GetSubsystem<Renderer>();
        auto viewport = MakeShared<Viewport>(context_, m_scene, m_camera->GetComponent<Camera>());
        renderer->SetViewport(0, viewport);
    }

    SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(App, handle_begin_frame));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(App, handle_key_down));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(App, handle_key_up));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(App, handle_update));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(App, handle_postrender_update));
    // SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(App, handle_post_update));
    // SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(App, handle_render_update));
    // SubscribeToEvent(E_ENDFRAME, URHO3D_HANDLER(App, handle_end_frame));
}

void App::Stop()
{
}

void App::handle_begin_frame(StringHash eventType, VariantMap& eventData)
{
}

void App::handle_key_down(StringHash eventType, VariantMap& eventData)
{
    int key = eventData[KeyDown::P_KEY].GetInt();
    switch (key) {
        case KEY_ESCAPE: {
            engine_->Exit();
            break;
        }
        case KEY_TAB: {
            const auto is_mouse_visible = GetSubsystem<Input>()->IsMouseVisible();
            GetSubsystem<Input>()->SetMouseVisible(!is_mouse_visible);
            auto hint_text = dynamic_cast<Text*>(GetSubsystem<UI>()->GetRoot()->GetChild(String("Hint Text")));
            hint_text->SetText(is_mouse_visible ? "Press WSAD to move around" : "Press TAB to show/hide mouse");
            break;
        }
    }
}

void App::handle_key_up(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    // int key = eventData[KeyDown::P_KEY].GetInt();
    // switch (key) {

    // }
}

void App::handle_update(StringHash eventType, VariantMap& eventData)
{
    const auto time_step = eventData[Urho3D::Update::P_TIMESTEP].GetFloat();
    static auto counter = 0.f;
    constexpr auto fps_update_time = 0.5f;  // in seconds
    if ((counter += time_step) > fps_update_time) {
        auto hint_text = dynamic_cast<Text*>(GetSubsystem<UI>()->GetRoot()->GetChild(String("FPS")));
        hint_text->SetText(ToString("FPS: %f", std::roundf(counter / time_step / fps_update_time)));
        counter = 0;
    }
    adjust_camera(time_step);
}

void App::adjust_camera(float time_step)
{
    auto input = GetSubsystem<Input>();
    // Do not move camera if cursor is visible or the UI has a focused element (e.g. the console)
    if (input->IsMouseVisible() || GetSubsystem<UI>()->GetFocusElement())
        return;

    // Movement speed as world units per second
    constexpr auto MOVE_SPEED = 20.f;
    // Mouse sensitivity as degrees per pixel
    constexpr auto MOUSE_SENSITIVITY = 0.1f;

    // Use this frame's mouse motion to adjust camera node yaw, pitch and roll. Clamp the pitch between -90 and 90 degrees
    auto mouse_moved = input->GetMouseMove();
    m_head.yaw += MOUSE_SENSITIVITY * mouse_moved.x_;
    m_head.pitch = Clamp(m_head.pitch += MOUSE_SENSITIVITY * mouse_moved.y_, -90.f, 90.f);
    m_camera->SetRotation(Quaternion(m_head.pitch, m_head.yaw, m_head.roll));

    if (input->GetKeyDown(KEY_W)) {
        m_camera->Translate(Vector3::FORWARD * MOVE_SPEED * time_step);
    }
    if (input->GetKeyDown(KEY_S)) {
        m_camera->Translate(Vector3::BACK * MOVE_SPEED * time_step);
    }
    if (input->GetKeyDown(KEY_A)) {
        m_camera->Translate(Vector3::LEFT * MOVE_SPEED * time_step);
    }
    if (input->GetKeyDown(KEY_D)) {
        m_camera->Translate(Vector3::RIGHT * MOVE_SPEED * time_step);
    }
}

void App::handle_postrender_update(StringHash eventType, VariantMap& eventData)
{
}

void App::handle_closed_pressed(StringHash eventType, VariantMap& eventData)
{
    engine_->Exit();
}

void App::init_user_interface()
{
    auto cache = GetSubsystem<ResourceCache>();
    auto ui = GetSubsystem<UI>();
    ui->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    auto quit_button = new Button(context_);
    quit_button->SetName("Quit Button");
    quit_button->SetStyle("Button");
    quit_button->SetSize(64, 64);
    quit_button->SetPosition(16, 116);
    ui->GetRoot()->AddChild(quit_button);
    SubscribeToEvent(quit_button, E_RELEASED, URHO3D_HANDLER(App, handle_closed_pressed));

    auto hint_text = new Text(context_);
    hint_text->SetName("Hint Text");
    hint_text->SetText("Press TAB to show/hide mouse");
    hint_text->SetFont(cache->GetResource<Font>("Fonts/gta5.ttf"), 50);
    hint_text->SetTextEffect(TextEffect::TE_STROKE);
    hint_text->SetEffectStrokeThickness(5);
    hint_text->SetEffectColor(Color(0.f, 0.f, 0.f));
    hint_text->SetColor(Color(1.f, 1.f, 1.f));
    hint_text->SetHorizontalAlignment(HA_CENTER);
    hint_text->SetVerticalAlignment(VA_TOP);
    ui->GetRoot()->AddChild(hint_text);

    auto text_fps = new Text(context_);
    text_fps->SetName("FPS");
    text_fps->SetFont(cache->GetResource<Font>("Fonts/gta5.ttf"), 50);
    text_fps->SetTextEffect(TextEffect::TE_STROKE);
    text_fps->SetEffectStrokeThickness(5);
    text_fps->SetEffectColor(Color(0.f, 0.f, 0.f));
    text_fps->SetColor(Color(1.f, 1.f, 1.f));
    text_fps->SetHorizontalAlignment(HorizontalAlignment::HA_LEFT);
    text_fps->SetVerticalAlignment(VerticalAlignment::VA_TOP);
    text_fps->SetPosition(40, 20);
    ui->GetRoot()->AddChild(text_fps);
}

URHO3D_DEFINE_APPLICATION_MAIN(App)
