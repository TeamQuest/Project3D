#pragma once

#include "Quests/Quest.hpp"

#include <Urho3D/Scene/LogicComponent.h>

#include <map>

namespace Urho3D {
    class Window;
}

using quest_map = std::map<Urho3D::String, Quest*>;

class QuestGiver : public Urho3D::LogicComponent {
    URHO3D_OBJECT(QuestGiver, LogicComponent);

public:
    explicit QuestGiver(Urho3D::Context* context);
    ~QuestGiver() override = default;

    void Start() override;
    void Update(float time_step) override;

    quest_map& get_quests();
    [[nodiscard]] const quest_map& get_quests() const;
    void assign_quest(Quest* quest);
    void setup_window(Urho3D::Window* window);

private:
    quest_map m_quests;
    Urho3D::Window* m_window{};
};
