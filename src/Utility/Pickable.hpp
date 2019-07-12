#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"
#include <Urho3D/Scene/LogicComponent.h>
#pragma clang diagnostic pop

#include <vector>

class Pickable : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Pickable, Urho3D::LogicComponent);

public:
    Pickable(Urho3D::Context* context);

    void set_item(const Urho3D::String& item);
    const Urho3D::String& item() const;

private:
    Urho3D::String m_item;
};
