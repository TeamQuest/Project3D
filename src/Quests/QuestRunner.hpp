#pragma once

#include "Quests/Quest.hpp"

#include <Urho3D/Scene/LogicComponent.h>

#include <map>

using quest_map = std::map<Urho3D::String, Quest*>;

class QuestRunner : public Urho3D::LogicComponent {
URHO3D_OBJECT(QuestRunner, LogicComponent);

public:
    explicit QuestRunner(Urho3D::Context*);
    ~QuestRunner() override = default;

    void Start() override;
    void Update(float time_step) override;

    quest_map& get_quests();
    [[nodiscard]] const quest_map& get_quests() const;

private:
    quest_map m_quests{};
};
