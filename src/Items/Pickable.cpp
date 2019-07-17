#include "Items/Pickable.hpp"

using namespace Urho3D;

Pickable::Pickable(Context* context) : LogicComponent(context)
{
}

void Pickable::set_name(const String& name)
{
    m_name = name;
}

const String& Pickable::get_name() const
{
    return m_name;
}

void Pickable::set_description(const String& description)
{
    m_description = description;
}

const String& Pickable::get_description() const
{
    return m_description;
}
