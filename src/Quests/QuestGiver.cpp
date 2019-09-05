#include "Quests/QuestGiver.hpp"

#include "Constants.hpp"
#include "Quests/QuestRunner.hpp"
#include "Utility/Common.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Menu.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/UI.h>
#pragma clang diagnostic pop

using namespace Urho3D;

QuestGiver::QuestGiver(Context* context) : LogicComponent(context)
{
}

void QuestGiver::Start()
{
    m_window = *make<Window>(context_).fixedsize(300, 300).styleauto().position(300, 300);
    m_window->SetLayout(LayoutMode::LM_FREE, 2, {5, 5, 5, 5});
    const auto anonymous_pro_font = GetSubsystem<ResourceCache>()->GetResource<Font>(("Fonts/Anonymous Pro.ttf"));
    {  /* Quest pop-up window */
        auto descr = *make<Text>(context_)
                .text("__MISSING_QUEST_DESCRIPTION__")
                .font(anonymous_pro_font)
                .fontsize(15)
                .position(0, 50)
                .alignment(HA_CENTER, VA_TOP)
                .textaligned(HA_LEFT)
                .name("QuestDesc");
        auto next_button = *make<Button>(context_)
                .styleauto()
                .alignment(HA_RIGHT, VA_BOTTOM)
                .fixedsize(30, 30);
        auto prev_button = *make<Button>(context_)
                .styleauto()
                .alignment(HA_LEFT, VA_BOTTOM)
                .fixedsize(30, 30);
        auto exit_button = *make<Button>(context_)
                .styleauto()
                .alignment(HA_LEFT, VA_TOP)
                .fixedsize(30, 30);
        auto left_arrow = *make<Text>(context_)
                .text("<-").styleauto()
                .aligned(HA_CENTER, VA_CENTER)
                .texteffect(TE_STROKE).effectcolor(Color::WHITE)
                .effectstrokethickness(1);
        auto right_arrow = *make<Text>(context_)
                .text("->").styleauto()
                .aligned(HA_CENTER, VA_CENTER)
                .texteffect(TE_STROKE).effectcolor(Color::WHITE)
                .effectstrokethickness(1);
        auto exit_x = *make<Text>(context_)
                .text("X").styleauto()
                .aligned(HA_CENTER, VA_CENTER);
        next_button->AddChild(right_arrow);
        prev_button->AddChild(left_arrow);
        exit_button->AddChild(exit_x);
        m_window->AddChild(descr);
        m_window->AddChild(next_button);
        m_window->AddChild(prev_button);
        m_window->AddChild(exit_button);
        SubscribeToEvent(next_button, E_RELEASED, [this](auto, auto) {
            auto quest = get<Quest>(m_window->GetVar("quest"));
            quest->move_page(1);
            if (quest->at_last_page()) {
                auto proceed_button = m_window->GetChildStaticCast<Button>("Accept", false);
                proceed_button->SetVisible(true);
            }
            m_window->GetChildStaticCast<Text>("QuestDesc", false)->SetText(quest->get_description());
        });
        SubscribeToEvent(prev_button, E_RELEASED, [this](auto, auto) {
            auto quest = get<Quest>(m_window->GetVar("quest"));
            if (quest->at_last_page()) {
                auto proceed_button = m_window->GetChildStaticCast<Button>("Accept", false);
                proceed_button->SetVisible(false);
            }
            quest->move_page(-1);
            m_window->GetChildStaticCast<Text>("QuestDesc", false)->SetText(quest->get_description());
        });
        SubscribeToEvent(exit_button, E_RELEASED, [this](auto, auto) {
            m_window->SetVisible(false);
        });
    }
    {  /* Proceed button */
        auto proceed_button = *make<Button>(context_)
                .styleauto()
                .alignment(HA_CENTER, VA_BOTTOM)
                .fixedsize(70, 30)
                .name("Accept");
        auto text = *make<Text>(context_)
                .aligned(HA_CENTER, VA_CENTER)
                .styleauto()
                .text("Accept");
        proceed_button->AddChild(text);
        proceed_button->SetVisible(false);
        m_window->AddChild(proceed_button);
        SubscribeToEvent(proceed_button, E_RELEASED, [this](auto, auto) {
            auto quest = get<Quest>(m_window->GetVar("quest"));
            auto quest_runner = GetScene()->GetChild(PLAYER_NAME)->GetComponent<QuestRunner>();
            quest->assign_to(quest_runner);
            get<Window>(m_window->GetVar("prev_window"))->SetVisible(false);
            m_window->SetVisible(false);
        });
    }
    m_window->SetVisible(false);

    GetSubsystem<UI>()->GetRoot()->AddChild(m_window);
}

void QuestGiver::Update(float /* time_step */)
{
    if (m_window->IsEnabled()) {

    }
}

quest_map& QuestGiver::get_quests()
{
    return m_quests;
}

const quest_map& QuestGiver::get_quests() const
{
    return m_quests;
}

void QuestGiver::setup_window(Window* window)
{
    window->SetEnabledRecursive(true);
    window->SetVisible(true);
    const auto window_title = *make<Text>(context_).styleauto().text("Quest Runner");
    window->AddChild(window_title);
    const auto anonymous_pro_font = GetSubsystem<ResourceCache>()->GetResource<Font>(("Fonts/Anonymous Pro.ttf"));

    for (auto [quest_name, quest] : m_quests) {
        auto quest_button = *make<Menu>(context_).styleauto().fixedheight(50).minwidth(300);
        auto quest_text = *make<Text>(context_)
                .name("qname")
                .text(quest_name)
                .font(anonymous_pro_font)
                .fontsize(15)
                .alignment(HA_CENTER, VA_CENTER)
                .textaligned(HA_CENTER);
        quest_button->AddChild(quest_text);
        quest_button->SetVar("quest", quest);
        set_button_based_on_stage(quest, quest_button);
        SubscribeToEvent(quest_button, E_RELEASED, [this](auto, auto event_data) {
            auto button = static_cast<Menu*>(event_data[Released::P_ELEMENT].GetPtr());
            auto quest = get<Quest>(button->GetVar("quest"));
            // Not too good...
            set_button_based_on_stage(quest, button);
            if (button->IsEnabled()) {
                m_window->GetChildStaticCast<Text>("QuestDesc", false)->SetText(quest->get_description());
                m_window->SetVisible(true);
                m_window->SetVar("quest", quest);
            }
        });
        window->AddChild(quest_button);
        m_window->SetVar("prev_window", window);
    }
}

void QuestGiver::assign_quest(Quest* quest) {
    m_quests.try_emplace(quest->get_name(), quest);
}
