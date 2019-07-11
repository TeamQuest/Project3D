#include "Utils/Pickable.hpp"
#include <iostream>

using namespace Urho3D;

Pickable::Pickable(Context* context) : LogicComponent(context)
{
}

/// Handle scene update. Called by LogicComponent base class.
void Pickable::Update(float timeStep)
{
}

void Pickable::SetMessage(String msg)
{
    m_msg = msg;
}

String Pickable::GetMessage() const
{
    return m_msg;
}