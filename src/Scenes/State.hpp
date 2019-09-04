#pragma once

#include "Scenes/Scenes.hpp"

#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>

class State : public Urho3D::Object {
    URHO3D_OBJECT(State, Object);

public:
    explicit State(Urho3D::Context*, Scenes type);
    virtual ~State();

    virtual void update(float time_step) = 0;

public:
    Urho3D::UniquePtr<Urho3D::Scene> scene;
    Scenes type;
};
