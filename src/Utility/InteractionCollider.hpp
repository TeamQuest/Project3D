#pragma once

#include <Urho3D/Scene/LogicComponent.h>

namespace Urho3D {
class Window;
}

class Pickable;
class QuestGiver;

class InteractionCollider : public Urho3D::LogicComponent {
    URHO3D_OBJECT(InteractionCollider, Urho3D::LogicComponent);

public:
    explicit InteractionCollider(Urho3D::Context*);

    void Start() override;
    void Update(float time_step) override;

    void handle_collision();
    void handle_collision_with_npc();
    void handle_interaction();
    void open_window();
    void close_window();

private:
    bool handle_item_clicked(const Urho3D::SharedPtr<Pickable>& item);

private:
    Urho3D::WeakPtr<Urho3D::Node> m_highlighted;
    Urho3D::Window* m_window{};
};
