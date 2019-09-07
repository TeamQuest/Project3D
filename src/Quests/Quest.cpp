#include "Quests/Quest.hpp"

#include "QuestRunner.hpp"

#include "Urho3D/UI/Text.h"

using namespace Urho3D;

Quest::Quest(Context* context, bool is_available)
    : Object(context),
      m_current_stage{is_available ? stage::available : stage::unavailable}
{
}

[[nodiscard]] const String& Quest::get_name() const
{
    return m_name;
}

[[nodiscard]] String Quest::get_description() const
{
    if (m_current_page < m_pages.size()) {
        return m_pages[m_current_page];
    }
    else {
        return "__MISSING_PAGE__";
    }
}

void Quest::move_page(int offset) {
    m_current_page = Clamp(m_current_page + offset, 0, static_cast<int>(m_pages.size()) - 1);
}

bool Quest::at_last_page() const {
    return m_current_page == m_pages.size() - 1;
}

void Quest::assign_to(QuestRunner* runner) {
    assert(runner);
    m_runner = runner;
    runner->assign_quest(this);
    m_current_stage = in_progress;
}

void Quest::unassign(bool set_available) {
    if (auto runner = m_runner.Lock()) {
        auto&& quests = runner->get_quests();
        quests.erase(m_name);
        m_current_stage = set_available ? available : unavailable;
    }
}

void set_button_based_on_stage(Quest* quest, Button* button) {
    auto quest_name = button->GetChildStaticCast<Text>("qname", false)->GetText();
    String suffix;
    switch (quest->m_current_stage) {
        case Quest::done:
            button->SetEnabled(false);
            button->SetVisible(false);
            suffix = " (done)";
            break;
        case Quest::unavailable:
            button->SetEnabled(false);
            suffix = " (unavailable)";
            break;
        case Quest::in_progress:
            button->SetEnabled(false);
            suffix = " (in progress)";
            break;
        case Quest::available:
        case Quest::failed:
        case Quest::timed_out:
            break;
    }
    button->GetChildStaticCast<Text>("qname", false)->SetText(quest_name + suffix);
}
