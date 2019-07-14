#include "Game.hpp"

#include "Scenes/Gameplay.hpp"
#include "Scenes/MainMenu.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
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
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>

#pragma clang diagnostic pop

using namespace Urho3D;

Game::Game(Urho3D::Context* context) : Application(context)
{
}

void Game::Setup()
{
    engineParameters_["FullScreen"] = false;
    engineParameters_["WindowWidth"] = 1280;
    engineParameters_["WindowHeight"] = 720;
    engineParameters_["WindowResizable"] = false;
    engineParameters_["VSync"] = true;
    engine_->SetMaxInactiveFps(20);
}

void Game::Start()
{
    m_active_state = new MainMenu(context_);
    assert(m_active_state);
    assert(m_active_state->scene);
    auto scene = m_active_state->scene;

    scene->CreateComponent<DebugRenderer>();
    scene->CreateComponent<Octree>();
    scene->CreateComponent<PhysicsWorld>();
    {
        m_camera = scene->CreateChild("Camera");
        auto camera = m_camera->CreateComponent<Camera>();
        camera->SetFarClip(2000);
    }
    {
        auto renderer = GetSubsystem<Renderer>();
        auto viewport = MakeShared<Viewport>(context_, m_active_state->scene, m_camera->GetComponent<Camera>());
        renderer->SetViewport(0, viewport);
    }

    SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(Game, handle_begin_frame));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Game, handle_key_down));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Game, handle_update));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Game, handle_postrender_update));
    // May not work when scene is changed (CHECK IT!)
    SubscribeToEvent(m_active_state->GetNode(), E_EXITREQUESTED, [&](auto, auto&) { engine_->Exit(); });
    SubscribeToEvent(E_STARTGAME, [&](auto&&...) {
        GetSubsystem<UI>()->GetRoot()->RemoveAllChildren();
        m_next_state = Scenes::Gameplay;
    });
    SubscribeToEvent(E_MENUREQUESTED, [&](auto&&...) {
        GetSubsystem<UI>()->GetRoot()->RemoveAllChildren();
        m_next_state = Scenes::MainMenu;
    });
}

void Game::Stop()
{
    engine_->Exit();
}

void Game::handle_begin_frame(StringHash /* event_type */, VariantMap& /* event_data */)
{
    switch (m_next_state) {
        case Scenes::Empty:
            return;
        case Scenes::MainMenu: {
            m_active_state = new MainMenu(context_);
            break;
        }
        case Scenes::Gameplay: {
            m_active_state = new Gameplay(context_);
            break;
        }
        default:
            assert(false);
            break;
    }
    m_next_state = Scenes::Empty;
}

void Game::handle_key_down(Urho3D::StringHash /* event_type */, Urho3D::VariantMap& event_data)
{
    int key = event_data[KeyDown::P_KEY].GetInt();
    switch (key) {
        case KEY_ESCAPE: {
            Stop();
            break;
        }
        case KEY_K: {
            // Something bad must happen here... MUAAHAHAHAHAHAHA!
            m_next_state = Scenes::Gameplay;
            break;
        }
        case KEY_TAB: {
            const auto is_mouse_visible = GetSubsystem<Input>()->IsMouseVisible();
            GetSubsystem<Input>()->SetMouseVisible(!is_mouse_visible);
            break;
        }
    }
}

void Game::handle_update(Urho3D::StringHash /* event_type */, Urho3D::VariantMap& event_data)
{
    const auto time_step = event_data[Urho3D::Update::P_TIMESTEP].GetFloat();
    m_active_state->Update(time_step);
}

void Game::handle_postrender_update(Urho3D::StringHash /* event_type */, Urho3D::VariantMap& /* event_data */)
{
}

void Game::handle_post_update(Urho3D::StringHash /* event_type */, Urho3D::VariantMap& /* event_data */)
{
}

URHO3D_DEFINE_APPLICATION_MAIN(Game)
