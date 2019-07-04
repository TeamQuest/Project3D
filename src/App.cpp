#include "App.hpp"

// #include <Urho3D/Graphics/StaticModel.h>
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

    auto cache = GetSubsystem<ResourceCache>();
    m_scene->CreateComponent<DebugRenderer>();
    m_scene->CreateComponent<Octree>();

    /* Button example */
    GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
    auto button = new Button(context_);
    GetSubsystem<UI>()->GetRoot()->AddChild(button);
    button->SetName("Button Quit");
    button->SetStyle("Button");
    button->SetSize(32, 32);
    button->SetPosition(16, 116);
    SubscribeToEvent(button, E_RELEASED, URHO3D_HANDLER(App, handle_closed_pressed));
    /* - */

    /* Text example */
    auto text = new Text(context_);
    text->SetText("Press TAB to show/hide mouse");
    text->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 40);
    text->SetColor(Color(0.f, 0.f, .3f));
    text->SetHorizontalAlignment(HA_CENTER);
    text->SetVerticalAlignment(VA_TOP);
    GetSubsystem<UI>()->GetRoot()->AddChild(text);
    /* - */

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
        Renderer* renderer = GetSubsystem<Renderer>();
        auto viewport = MakeShared<Viewport>(context_, m_scene, m_camera->GetComponent<Camera>());
        renderer->SetViewport(0, viewport);
    }

    SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(App, handle_begin_frame));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(App, handle_key_down));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(App, handle_update));
    // SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(App, handle_post_update));
    // SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(App, handle_render_update));
    // SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(App, handle_post_render_update));
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
        case KEY_ESCAPE:
            engine_->Exit();
            break;
        case KEY_TAB:
            const auto is_mouse_visible = GetSubsystem<Input>()->IsMouseVisible();
            GetSubsystem<Input>()->SetMouseVisible(!is_mouse_visible);
            break;
    }
}

void App::handle_update(StringHash eventType, VariantMap& eventData)
{
    // code
}

void App::handle_closed_pressed(StringHash eventType, VariantMap& eventData)
{
    engine_->Exit();
}

URHO3D_DEFINE_APPLICATION_MAIN(App)
