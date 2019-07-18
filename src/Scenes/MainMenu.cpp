#include "Scenes/MainMenu.hpp"
#include "Utility/Common.hpp"
#include "Utility/FPSCounter.hpp"

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

    {  // setup_scene_components
        scene->CreateComponent<FPSCounter>();
    }

    const auto cache = GetSubsystem<ResourceCache>();
    auto ui_root = GetSubsystem<UI>()->GetRoot();
    ui_root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    {  // StartGame button and label
        auto start_game_button = *make<Button>(context)
                                      .name("StartGameButton")
                                      .style("Button")
                                      .size(500, 100)
                                      .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER)
                                      .position(0, -220);

        auto start_game_label = *make<Text>(context)
                                     .name("StartGameLabel")
                                     .text("start new game")
                                     .font(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 30)
                                     .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER)
                                     .textalignment(HorizontalAlignment::HA_CENTER);
        start_game_button->AddChild(start_game_label);
        ui_root->AddChild(start_game_button);
    }
    {  // Options button and label
        auto options_button = *make<Button>(context)
                                   .name("OptionsButton")
                                   .style("Button")
                                   .size(500, 100)
                                   .position(0, -110)
                                   .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER);

        auto options_button_label = *make<Text>(context)
                                         .name("Options")
                                         .text("options")
                                         .font(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 30)
                                         .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER)
                                         .textalignment(HorizontalAlignment::HA_CENTER);
        options_button->AddChild(options_button_label);
        ui_root->AddChild(options_button);
    }
    {  // Quit button and label
        auto quit_button = *make<Button>(context)
                                .name("QuitButton")
                                .style("Button")
                                .size(500, 100)
                                .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER);
        auto quit_button_label = *make<Text>(context)
                                      .name("QuitLabel")
                                      .text("quit game")
                                      .font(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 30)
                                      .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER)
                                      .textalignment(HorizontalAlignment::HA_CENTER);
        quit_button->AddChild(quit_button_label);
        ui_root->AddChild(quit_button);
    }

    SubscribeToEvent(ui_root->GetChild("StartGameButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_STARTGAME); });
    SubscribeToEvent(ui_root->GetChild("OptionsButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_OPTIONSREQUESTED); });
    SubscribeToEvent(ui_root->GetChild("QuitButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_EXITREQUESTED); });
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MainMenu, handle_key_down));
}

void MainMenu::update(float /* time_step */)
{
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
