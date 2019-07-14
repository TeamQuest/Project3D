#include "Gameplay.hpp"

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

#include <cmath>

using namespace Urho3D;

Gameplay::Gameplay(Context* context) : State(context)
{
    URHO3D_LOGWARNING("Setting up Gameplay scene...");
    const auto cache = GetSubsystem<ResourceCache>();
    auto ui = GetSubsystem<UI>();
    ui->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

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

    SubscribeToEvent(quit_button, E_RELEASED, [&](StringHash, VariantMap&) {
        SendEvent(E_MENUREQUESTED);
        URHO3D_LOGWARNING("Exitting Gameplay scene...");
    });

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
}
