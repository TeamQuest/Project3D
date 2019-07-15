#include "Scenes/Options.hpp"

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

Options::Options(Context* context) : State(context, Scenes::Options)
{
    URHO3D_LOGINFO("Options scene enabled");

    const auto cache = GetSubsystem<ResourceCache>();
    auto ui_root = GetSubsystem<UI>()->GetRoot();
    ui_root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    {  // Quit button and label
        auto back_to_menu_button = new Button(context);
        back_to_menu_button->SetName("BackToMenuButton");
        back_to_menu_button->SetStyle("Button");
        back_to_menu_button->SetSize(500, 100);
        back_to_menu_button->SetAlignment(HA_CENTER, VA_CENTER);

        auto back_to_main_button_label = new Text(context);
        back_to_main_button_label->SetName("BackToMenuButtonLabel");
        back_to_main_button_label->SetText("back to main");
        back_to_main_button_label->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 30);
        back_to_main_button_label->SetAlignment(HA_CENTER, VA_CENTER);
        back_to_main_button_label->SetTextAlignment(HA_CENTER);
        back_to_menu_button->AddChild(back_to_main_button_label);
        ui_root->AddChild(back_to_menu_button);
    }

    SubscribeToEvent(ui_root->GetChild("BackToMenuButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_MENUREQUESTED); });
}

void Options::update(float /* time_step */)
{
}

Options::~Options()
{
    URHO3D_LOGINFO("Options scene disabled");
}
