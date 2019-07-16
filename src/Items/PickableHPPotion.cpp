#include "Items/PickableHPPotion.hpp"
#include "Items/Pickable.hpp"

using namespace Urho3D;

PickableHPPotion::PickableHPPotion(Context* context) : Pickable(context)
{
}

void PickableHPPotion::set_name(const String& name)
{
    m_name = name;
}

const String& PickableHPPotion::get_name() const
{
    return m_name;
}

void PickableHPPotion::set_description(const String& description)
{
    m_description = description;
}

const String& PickableHPPotion::get_desctiption() const
{
    return m_description;
}
