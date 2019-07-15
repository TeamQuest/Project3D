#include "Scenes/State.hpp"

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
