#include "Items/Pickable.hpp"

using namespace Urho3D;

Pickable::Pickable(Context* context) : LogicComponent(context)
{
}

void Pickable::set_name(const String& item_name)
{
    m_name = item_name;
}

const String& Pickable::name() const
{
    return m_name;
}
