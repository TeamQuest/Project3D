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
#include <Urho3D/Input/Input.h>
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

/**
 * This method is called before the engine has been initialized.
 * Thusly, we can setup the engine parameters before anything else
 * of engine importance happens (such as windows, search paths,
 * resolution and other things that might be user configurable).
 */
void App::Setup()
{
    engineParameters_["FullScreen"] = false;
    engineParameters_["WindowWidth"] = 1280;
    engineParameters_["WindowHeight"] = 720;
    engineParameters_["WindowResizable"] = false;
    engine_->SetMaxFps(60);
    engine_->SetMaxInactiveFps(20);
}

void App::Start()
{
    GetSubsystem<Input>()->SetMouseVisible(true);

    const auto cache = GetSubsystem<ResourceCache>();
    m_scene->CreateComponent<DebugRenderer>();
    m_scene->CreateComponent<Octree>();

    /* Button example */
    GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
    auto button = new Button(context_);
    GetSubsystem<UI>()->GetRoot()->AddChild(button);
    button->SetName("Button Quit");
    button->SetStyle("Button");
    button->SetSize(64, 64);
    button->SetPosition(16, 116);
    SubscribeToEvent(button, E_RELEASED, URHO3D_HANDLER(App, handle_closed_pressed));
    /* - */

    /* Text example */
    auto text = new Text(context_);
    text->SetName("instrukcja");
    text->SetText("Press TAB to show/hide mouse");
    text->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 40);
    text->SetColor(Color(0.f, 0.f, .3f));
    text->SetHorizontalAlignment(HA_CENTER);
    text->SetVerticalAlignment(VA_TOP);
    GetSubsystem<UI>()->GetRoot()->AddChild(text);
    /* - */

    constexpr auto NUM_OBJECTS = 2000u;
    for (unsigned i = 0; i < NUM_OBJECTS; ++i) {
        auto box = m_scene->CreateChild("Box");
        box->SetPosition(Vector3(Random(200.0f) - 100.0f, Random(200.0f) - 100.0f, Random(200.0f) - 100.0f));
        box->SetRotation(Quaternion(Random(360.0f), Random(360.0f), Random(360.0f)));
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
        light->SetRange(30.0f);
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
            auto text = dynamic_cast<Text*>(GetSubsystem<UI>()->GetRoot()->GetChild(String("instrukcja")));
            text->SetText(is_mouse_visible ? "Press WSAD to move around" : "Press TAB to show/hide mouse");
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
    adjust_camera(time_step);
}

void App::adjust_camera(float time_step)
{
    auto input = GetSubsystem<Input>();
    // Do not move camera if cursor is visible or the UI has a focused element (e.g. the console)
    if (input->IsMouseVisible() || GetSubsystem<UI>()->GetFocusElement())
        return;

    // Movement speed as world units per second
    constexpr auto MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    constexpr auto MOUSE_SENSITIVITY = 0.1f;

    // Use this frame's mouse motion to adjust camera node yaw, pitch and roll. Clamp the pitch between -90 and 90 degrees
    auto mouse_moved = input->GetMouseMove();
    m_head.yaw += MOUSE_SENSITIVITY * mouse_moved.x_;
    m_head.pitch = Clamp(m_head.pitch += MOUSE_SENSITIVITY * mouse_moved.y_, -90.0f, 90.0f);
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

URHO3D_DEFINE_APPLICATION_MAIN(App)
