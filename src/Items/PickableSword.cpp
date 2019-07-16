#include "Items/PickableSword.hpp"
#include "Items/Pickable.hpp"

using namespace Urho3D;

PickableSword::PickableSword(Context* context) : Pickable(context)
{
}

void PickableSword::set_name(const String& name)
{
    m_name = name;
}

const String& PickableSword::get_name() const
{
    return m_name;
}

void PickableSword::set_description(const String& description)
{
    m_description = description;
}

const String& PickableSword::get_desctiption() const
{
    return m_description;
}

void PickableSword::set_dmg(int value)
{
    m_dmg = value;
}

const int& PickableSword::get_dmg() const
{
    return m_dmg;
}
