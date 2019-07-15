#include "Scenes/MainMenu.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>

#pragma clang diagnostic pop

using namespace Urho3D;

MainMenu::MainMenu(Context* context) : State(context, Scenes::MainMenu)
{
    URHO3D_LOGINFO("MainMenu scene enabled");
    const auto cache = GetSubsystem<ResourceCache>();
    auto ui_root = GetSubsystem<UI>()->GetRoot();
    ui_root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    {  // StartGame button and label
        auto start_game_button = new Button(context);
        start_game_button->SetName("StartGameButton");
        start_game_button->SetStyle("Button");
        start_game_button->SetSize(200, 100);
        start_game_button->SetAlignment(HA_CENTER, VA_CENTER);
        start_game_button->SetPosition(0, -110);
        auto start_game_label = new Text(context);
        start_game_label->SetName("StartGameLabel");
        start_game_label->SetText("start new game");
        start_game_label->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 17);
        start_game_label->SetAlignment(HA_CENTER, VA_CENTER);
        start_game_label->SetTextAlignment(HA_CENTER);
        start_game_button->AddChild(start_game_label);
        ui_root->AddChild(start_game_button);
    }
    {  // Quit button and label
        auto quit_button = new Button(context);
        quit_button->SetName("QuitButton");
        quit_button->SetStyle("Button");
        quit_button->SetSize(200, 100);
        quit_button->SetAlignment(HA_CENTER, VA_CENTER);
        auto quit_button_label = new Text(context);
        quit_button_label->SetName("QuitLabel");
        quit_button_label->SetText("quit game");
        quit_button_label->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 17);
        quit_button_label->SetAlignment(HA_CENTER, VA_CENTER);
        quit_button_label->SetTextAlignment(HA_CENTER);
        quit_button->AddChild(quit_button_label);
        ui_root->AddChild(quit_button);
    }
    {  // FPS display
        auto fps_text = new Text(context);
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

    SubscribeToEvent(ui_root->GetChild("StartGameButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_STARTGAME); });
    SubscribeToEvent(ui_root->GetChild("QuitButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_EXITREQUESTED); });
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MainMenu, handle_key_down));
}

void MainMenu::Update(float time_step)
{
    static auto counter = 0.f;
    constexpr auto fps_update_time = 0.5f;  // in seconds
    if ((counter += time_step) > fps_update_time) {
        auto fps_text = static_cast<Text*>(GetSubsystem<UI>()->GetRoot()->GetChild(String("FPS")));
        fps_text->SetText(ToString("FPS: %f", std::roundf(counter / time_step / fps_update_time)));
        counter = 0;
    }
}

MainMenu::~MainMenu()
{
    URHO3D_LOGINFO("MainMenu scene disabled");
}

void MainMenu::handle_key_down(StringHash /* event_type */, VariantMap& event_data)
{
    const auto key = event_data[KeyDown::P_KEY].GetInt();
    switch (key) {
        case KEY_TAB: {
            const auto is_mouse_visible = GetSubsystem<Input>()->IsMouseVisible();
            GetSubsystem<Input>()->SetMouseVisible(!is_mouse_visible);
            break;
        }
    }
}
