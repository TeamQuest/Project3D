#pragma once

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/LogicComponent.h>

class State : public Urho3D::LogicComponent {
    URHO3D_OBJECT(State, LogicComponent);

public:
    State(Urho3D::Context*);
    virtual ~State();

    void Update(float time_step) override;

    static void register_component(Urho3D::Context* context);
    void activate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void deactivate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    
public:
    Urho3D::SharedPtr<Urho3D::Scene> scene;
};
