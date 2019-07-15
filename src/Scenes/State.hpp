#pragma once

#include "Scenes/Scenes.hpp"

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>

class State : public Urho3D::LogicComponent {
    URHO3D_OBJECT(State, LogicComponent);

public:
    State(Urho3D::Context*, Scenes type);
    virtual ~State();

    void Update(float time_step) override;

public:
    Urho3D::UniquePtr<Urho3D::Scene> scene;
    Scenes type;
};
