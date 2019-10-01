#include "Quests/QuestRunner.hpp"

#include "Urho3D/IO/Log.h"

using namespace Urho3D;

QuestRunner::QuestRunner(Context* context) : LogicComponent(context) {

}

void QuestRunner::Start() {

}

void QuestRunner::Update(float /* time_step */) {

}

quest_map& QuestRunner::get_quests() {
    return m_quests;
}

const quest_map& QuestRunner::get_quests() const {
    return m_quests;
}

void QuestRunner::assign_quest(Quest* quest) {
    m_quests.try_emplace(quest->get_name(), quest);
    URHO3D_LOGWARNINGF(R"(A new quest assigned: "%s". Pop-up window should show up)", quest->get_name().CString());
    // put logic here to inform HUD about a new quest
}
