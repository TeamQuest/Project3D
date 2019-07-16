#include "Scenes/State.hpp"

using namespace Urho3D;

State::State(Context* context, Scenes type) : Object(context), scene(new Scene(context)), type(type)
{
}

State::~State()
{
}
