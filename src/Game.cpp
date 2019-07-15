#include "Game.hpp"

#include "Scenes/Gameplay.hpp"
#include "Scenes/MainMenu.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/UI.h>

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
    GetSubsystem<Input>()->SetMouseVisible(true);
}

void Game::Start()
{
    m_active_state = new MainMenu(context_);
    assert(m_active_state);
    assert(m_active_state->scene);

    SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(Game, handle_change_state));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Game, handle_key_down));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Game, handle_update));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Game, handle_postrender_update));
    SubscribeToEvent(E_EXITREQUESTED, [&](auto&&...) { engine_->Exit(); });
    SubscribeToEvent(E_STARTGAME, [&](auto&&...) { m_next_state = Scenes::Gameplay; });
    SubscribeToEvent(E_MENUREQUESTED, [&](auto&&...) { m_next_state = Scenes::MainMenu; });
}

void Game::Stop()
{
    engine_->Exit();
}

void Game::handle_change_state(StringHash /* event_type */, VariantMap& /* event_data */)
{
    if (m_next_state == Scenes::Empty) {
        return;
    }
    GetSubsystem<UI>()->GetRoot()->RemoveAllChildren();
    m_active_state.Reset();
    switch (m_next_state) {
        case Scenes::MainMenu: {
            m_active_state = new MainMenu(context_);
            break;
        }
        case Scenes::Gameplay: {
            m_active_state = new Gameplay(context_);
            break;
        }
        default:
            // Unhandled scene caught
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
