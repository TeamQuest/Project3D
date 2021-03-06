#include "Game.hpp"

#include "Character/Status.hpp"
#include "Items/Gold.hpp"
#include "Items/HpPotion.hpp"
#include "Items/Inventory.hpp"
#include "Character/Npc.hpp"
#include "Items/Lootable.hpp"
#include "Items/Pickable.hpp"
#include "Items/Sword.hpp"
#include "HUD/Hud.hpp"
#include "Quests/QuestGiver.hpp"
#include "Quests/QuestRunner.hpp"
#include "Scenes/Gameover.hpp"
#include "Scenes/Gameplay.hpp"
#include "Scenes/MainMenu.hpp"
#include "Scenes/Options.hpp"
#include "Utility/Common.hpp"
#include "Utility/FPSCounter.hpp"
#include "Utility/InteractionCollider.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/UI.h>
#include <Enemies/Enemy.hpp>

#pragma clang diagnostic pop

using namespace Urho3D;

Game::Game(Context* context) : Application(context)
{
    // Component Register
    register_component<FPSCounter>(context);
    register_component<Character>(context);
    register_component<HpPotion>(context);
    register_component<Sword>(context);
    register_component<Gold>(context);
    register_component<Npc>(context);
    register_component<Pickable>(context);
    register_component<InteractionCollider>(context);
    register_component<Lootable>(context);
    register_component<Inventory>(context);
    register_component<QuestGiver>(context);
    register_component<QuestRunner>(context);
    register_component<Hud>(context);
    register_component<Status>(context);
    register_component<Enemy>(context);
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
    SubscribeToEvent(E_OPTIONSREQUESTED, [&](auto&&...) { m_next_state = Scenes::Options; });
    SubscribeToEvent(E_GAMEOVERREQUESTED, [&](auto&&...) { m_next_state = Scenes::GameOver; });
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
        case Scenes::Options: {
            m_active_state = new Options(context_);
            break;
        }
        case Scenes::Gameplay: {
            m_active_state = new Gameplay(context_);
            break;
        }
        case Scenes::GameOver: {
            m_active_state = new Gameover(context_);
            break;
        }
        default:
            // Unhandled scene caught
            assert(false);
            break;
    }
    m_next_state = Scenes::Empty;
}

void Game::handle_key_down(StringHash /* event_type */, VariantMap& event_data)
{
    int key = event_data[KeyDown::P_KEY].GetInt();
    switch (key) {
        case KEY_ESCAPE: {
            Stop();
            break;
        }
    }
}

void Game::handle_update(StringHash /* event_type */, VariantMap& event_data)
{
    const auto time_step = event_data[Update::P_TIMESTEP].GetFloat();
    m_active_state->update(time_step);
}

void Game::handle_postrender_update(StringHash /* event_type */, VariantMap& /* event_data */)
{
}

void Game::handle_post_update(StringHash /* event_type */, VariantMap& /* event_data */)
{
}

URHO3D_DEFINE_APPLICATION_MAIN(Game)
