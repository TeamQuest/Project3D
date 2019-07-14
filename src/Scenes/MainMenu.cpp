#include "Scenes/MainMenu.hpp"

#include "Scenes/Scenes.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
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

using namespace Urho3D;

MainMenu::MainMenu(Context* context) : State(context)
{
    const auto cache = GetSubsystem<ResourceCache>();
    auto ui = GetSubsystem<UI>();
    ui->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    // ======= New Game Start Button
    auto start_new_game_label = new Text(context);
    start_new_game_label->SetName("New Game Button Label");
    start_new_game_label->SetText("Start New Game");
    start_new_game_label->SetFont(cache->GetResource<Font>("Fonts/gta5.ttf"), 30);
    start_new_game_label->SetStyleAuto();
    start_new_game_label->SetTextAlignment(HA_CENTER);

    auto start_new_game_button = new Button(context);
    start_new_game_button->SetName("New Game Button");
    start_new_game_button->SetStyle("Button");
    start_new_game_button->SetSize(200, 100);
    start_new_game_button->SetAlignment(HA_CENTER, VA_CENTER);
    start_new_game_button->SetPosition(0, -110);
    start_new_game_button->AddChild(start_new_game_label);
    ui->GetRoot()->AddChild(start_new_game_button);
    SubscribeToEvent(start_new_game_button, E_RELEASED, [&](auto&&...) { SendEvent(E_STARTGAME); });

    // ======= Quit Button
    auto quit_button_label = new Text(context);
    quit_button_label->SetName("Quit Button Label");
    quit_button_label->SetText("Quit Button");
    quit_button_label->SetFont(cache->GetResource<Font>("Fonts/gta5.ttf"), 30);
    quit_button_label->SetStyleAuto();
    quit_button_label->SetTextAlignment(HA_CENTER);

    auto quit_button = new Button(context);
    quit_button->SetName("Quit Button");
    quit_button->SetStyle("Button");
    quit_button->SetSize(200, 100);
    quit_button->SetAlignment(HA_CENTER, VA_CENTER);
    quit_button->AddChild(quit_button_label);
    ui->GetRoot()->AddChild(quit_button);

    SubscribeToEvent(quit_button, E_RELEASED, URHO3D_HANDLER(MainMenu, handle_closed_pressed));

    // auto quit_button = new Button(context_);
    // quit_button->SetName("Quit Button");
    // quit_button->SetStyle("Button");
    // quit_button->SetSize(64, 64);
    // quit_button->SetPosition(16, 116);
    // ui->GetRoot()->AddChild(quit_button);
    // SubscribeToEvent(quit_button, E_RELEASED, URHO3D_HANDLER(App, handle_closed_pressed));

    // auto hint_text = new Text(context_);
    // hint_text->SetName("Hint Text");
    // hint_text->SetText("Press TAB to show/hide mouse");
    // hint_text->SetFont(cache->GetResource<Font>("Fonts/gta5.ttf"), 50);
    // hint_text->SetTextEffect(TextEffect::TE_STROKE);
    // hint_text->SetEffectStrokeThickness(5);
    // hint_text->SetEffectColor(Color(0.f, 0.f, 0.f));
    // hint_text->SetColor(Color(1.f, 1.f, 1.f));
    // hint_text->SetHorizontalAlignment(HA_CENTER);
    // hint_text->SetVerticalAlignment(VA_TOP);
    // ui->GetRoot()->AddChild(hint_text);

    auto fps_text = new Text(context_);
    fps_text->SetName("FPS");
    fps_text->SetFont(cache->GetResource<Font>("Fonts/gta5.ttf"), 50);
    fps_text->SetTextEffect(TextEffect::TE_STROKE);
    fps_text->SetEffectStrokeThickness(5);
    fps_text->SetEffectColor(Color(0.f, 0.f, 0.f));
    fps_text->SetColor(Color(1.f, 1.f, 1.f));
    fps_text->SetHorizontalAlignment(HorizontalAlignment::HA_LEFT);
    fps_text->SetVerticalAlignment(VerticalAlignment::VA_TOP);
    fps_text->SetPosition(40, 20);
    ui->GetRoot()->AddChild(fps_text);
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

void MainMenu::handle_closed_pressed(Urho3D::StringHash /* event_type */, Urho3D::VariantMap& /* event_data */)
{
    SendEvent(E_EXITREQUESTED);
}
