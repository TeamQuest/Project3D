#include "Gameplay.hpp"

#include "Utility/FPSCounter.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>

#pragma clang diagnostic pop

#include <cmath>

using namespace Urho3D;

Gameplay::Gameplay(Context* context) : State(context, Scenes::Gameplay)
{
    URHO3D_LOGINFO("Gameplay scene enabled");

    assert(context == context_);  // If that failed context would have to be passed to init_* functions
    init_ui();
    init_gamescene();
}

void Gameplay::init_ui()
{
    {  // setup_scene_components
        scene->CreateComponent<FPSCounter>();
    }
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

    SubscribeToEvent(ui_root->GetChild("ExitGameButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_MENUREQUESTED); });
}

void Gameplay::init_gamescene()
{
    {  // setup_scene_components
        scene->CreateComponent<DebugRenderer>();
        scene->CreateComponent<Octree>();
        scene->CreateComponent<PhysicsWorld>();
        m_camera = scene->CreateChild("Camera");
        auto camera = m_camera->CreateComponent<Camera>();
        camera->SetFarClip(2000);
    }
    const auto cache = GetSubsystem<ResourceCache>();
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

void Gameplay::Update(float /* time_step */)
{
}

Gameplay::~Gameplay()
{
    URHO3D_LOGINFO("Gameplay scene disabled");
}
