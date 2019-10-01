#include "Scenes/Options.hpp"

#include "Utility/Common.hpp"

#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/LineEdit.h>
#include <Character/Status.hpp>

using namespace Urho3D;

Options::Options(Context* context) : State(context, Scenes::Options)
{
    URHO3D_LOGINFO("Options scene enabled");

    const auto cache = GetSubsystem<ResourceCache>();
    auto ui_root = GetSubsystem<UI>()->GetRoot();
    ui_root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    {
        auto set_character_name_label = *make<Text>(context_)
                .name("SetCharacterName")
                .font(GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 30)
                .text("enter name for your character")
                .texteffect(TextEffect::TE_STROKE)
                .effectstrokethickness(3)
                .effectcolor(Color(0.f, 0.f, 0.f))
                .color(Color(1.f, 1.f, 1.f))
                .alignment(HA_CENTER, VA_CENTER)
                .position(0, -330);
        ui_root->AddChild(set_character_name_label);
    }
    {
        auto character_name_line_edit = *make<LineEdit>(context_)
                .name("SetCharacterNameLineEdit")
                .size(500, 100)
                .color(Color(0.6f, 0.6f, 0.6f))
                .alignment(HA_CENTER, VA_CENTER)
                .position(0, -220);

        character_name_line_edit->SetMinHeight(24);
        character_name_line_edit->GetTextElement()->SetText("Name...");
        character_name_line_edit->GetTextElement()->SetFont(GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 30);
        character_name_line_edit->GetTextElement()->SetEffectColor(Color(0.f, 0.f, 0.f));
        character_name_line_edit->SetEditable(true);
        character_name_line_edit->SetEnabled(true);
        character_name_line_edit->SetCursorBlinkRate(0.7f);
        character_name_line_edit->SetVar("entered_text", character_name_line_edit->GetText());
        ui_root->AddChild(character_name_line_edit);

        auto apply_name_btn = *make<Button>(context)
                .name("ApplyNameButton")
                .style("Button")
                .size(500, 100)
                .alignment(HA_CENTER, VA_CENTER)
                .position(0, -110);

        auto apply_name_btn_label = *make<Text>(context)
                .name("ApplyNameButtonLabel")
                .text("apply name")
                .font(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 30)
                .alignment(HA_CENTER, VA_CENTER)
                .textalignment(HA_CENTER);

        apply_name_btn->AddChild(apply_name_btn_label);
        ui_root->AddChild(apply_name_btn);

    }

    {  // back to menu button and label
        auto back_to_menu_button = *make<Button>(context)
                .name("BackToMenuButton")
                .style("Button")
                .size(500, 100)
                .alignment(HA_CENTER, VA_CENTER);

        auto back_to_main_button_label = *make<Text>(context)
            .name("BackToMenuButtonLabel")
            .text("back to main")
            .font(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 30)
            .alignment(HA_CENTER, VA_CENTER)
            .textalignment(HA_CENTER);
        back_to_menu_button->AddChild(back_to_main_button_label);
        ui_root->AddChild(back_to_menu_button);
    }

    SubscribeToEvent(ui_root->GetChild("BackToMenuButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_MENUREQUESTED); });
    SubscribeToEvent(ui_root->GetChild("ApplyNameButton", false), E_RELEASED, [&](auto&&...) {
        auto ui_root = GetSubsystem<UI>()->GetRoot();
        if (auto line_edit = ui_root->GetChildStaticCast<LineEdit>("SetCharacterNameLineEdit", false)) {
            auto&& player_name = line_edit->GetTextElement()->GetText();
            SetGlobalVar("PlayerName", player_name);
            SendEvent(E_MENUREQUESTED);
        }
    });
}

void Options::update(float /* time_step */)
{
}

Options::~Options()
{
    URHO3D_LOGINFO("Options scene disabled");
}
