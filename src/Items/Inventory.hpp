#pragma once

#include "Items/Pickable.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Container/Ptr.h>

#include <vector>

namespace Urho3D {
class Window;
}

class Inventory : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Inventory, Urho3D::LogicComponent);

public:
    explicit Inventory(Urho3D::Context* context);

    void Start() override;
    void Update(float time_step) override;

    bool add(const Urho3D::SharedPtr<Pickable>& pickable);
    void remove(Pickable* pickable);
    bool is_full() const;

    void toggle(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);

private:
    std::vector<Urho3D::SharedPtr<Pickable>> m_items{};
    Urho3D::Window* m_window;
    Urho3D::Window* m_window_description;
    int m_capacity = 5;

    void toggle_key_down(Urho3D::StringHash, Urho3D::VariantMap &);
};
