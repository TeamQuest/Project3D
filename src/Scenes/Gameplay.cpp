#include "Gameplay.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>

#pragma clang diagnostic pop

#include <cmath>

using namespace Urho3D;

Gameplay::Gameplay(Context* context) : State(context, Scenes::Gameplay)
{
    URHO3D_LOGINFO("Gameplay scene enabled");

    assert(context == context_);
    init_ui();
    init_gamescene();
}

void Gameplay::init_ui()
{
    const auto cache = GetSubsystem<ResourceCache>();
    auto ui_root = GetSubsystem<UI>()->GetRoot();
    ui_root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    {  // ExitGame button and label
        auto exit_game_button = new Button(context_);
        exit_game_button->SetName("ExitGameButton");
        exit_game_button->SetStyle("Button");
        exit_game_button->SetSize(100, 60);
        exit_game_button->SetAlignment(HA_LEFT, VA_TOP);
        exit_game_button->SetPosition(100, 100);
        auto exit_game_label = new Text(context_);
        exit_game_label->SetName("ExitGameLabel");
        exit_game_label->SetText("exit");
        exit_game_label->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 17);
        exit_game_label->SetAlignment(HA_CENTER, VA_CENTER);
        exit_game_button->AddChild(exit_game_label);
        ui_root->AddChild(exit_game_button);
    }
    {  // FPS display
        auto fps_text = new Text(context_);
        fps_text->SetName("FPS");
        fps_text->SetFont(cache->GetResource<Font>("Fonts/gta5.ttf"), 50);
        fps_text->SetTextEffect(TextEffect::TE_STROKE);
        fps_text->SetEffectStrokeThickness(5);
        fps_text->SetEffectColor(Color(0.f, 0.f, 0.f));
        fps_text->SetColor(Color(1.f, 1.f, 1.f));
        fps_text->SetAlignment(HA_LEFT, VA_TOP);
        fps_text->SetPosition(40, 20);
        ui_root->AddChild(fps_text);
    }

    SubscribeToEvent(ui_root->GetChild("ExitGameButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_MENUREQUESTED); });
}

void Gameplay::init_gamescene()
{
    const auto cache = GetSubsystem<ResourceCache>();
    {  // setup_scene_components
        scene->CreateComponent<DebugRenderer>();
        scene->CreateComponent<Octree>();
        scene->CreateComponent<PhysicsWorld>();
        m_camera = scene->CreateChild("Camera");
        auto camera = m_camera->CreateComponent<Camera>();
        camera->SetFarClip(2000);
    }
    {  // setup_viewport
        auto renderer = GetSubsystem<Renderer>();
        auto viewport = MakeShared<Viewport>(context_, scene.Get(), m_camera->GetComponent<Camera>());
        renderer->SetViewport(0, viewport);
    }
    {
        auto sky = scene->CreateChild("Sky");
        auto skybox = sky->CreateComponent<Skybox>();
        skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));
    }
}

void Gameplay::Update(float time_step)
{
    static auto counter = 0.f;
    constexpr auto fps_update_time = 0.5f;  // in seconds
    if ((counter += time_step) > fps_update_time) {
        auto fps_text = static_cast<Text*>(GetSubsystem<UI>()->GetRoot()->GetChild(String("FPS")));
        fps_text->SetText(ToString("FPS: %f", std::roundf(counter / time_step / fps_update_time)));
        counter = 0;
    }
}

Gameplay::~Gameplay()
{
    URHO3D_LOGINFO("Gameplay scene disabled");
}
