#include "Scenes/State.hpp"

#include <Urho3D/Scene/Node.h>

using namespace Urho3D;

State::State(Context* context, Scenes type) : LogicComponent(context), scene(new Scene(context)), type(type)
{
}

State::~State()
{
}

void State::Update(float time_step)
{
    (void)time_step;
}

void State::activate(StringHash, VariantMap&)
{
    node_->SetEnabledRecursive(true);
    // SetUpdateEventMask(USE_UPDATE);
}
void State::deactivate(StringHash, VariantMap&)
{
    node_->SetEnabledRecursive(false);
    // SetUpdateEventMask(USE_NO_EVENT);
}
