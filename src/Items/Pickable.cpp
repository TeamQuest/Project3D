#include "Items/Pickable.hpp"

using namespace Urho3D;

Pickable::Pickable(Context* context) : LogicComponent(context)
{
}

void Pickable::set_item(const String& item)
{
    m_item = item;
}

const String& Pickable::item() const
{
    return m_item;
}
