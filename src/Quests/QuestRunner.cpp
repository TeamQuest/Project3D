#include "Quests/QuestRunner.hpp"

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
