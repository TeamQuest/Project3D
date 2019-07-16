#include "Items/PickableGold.hpp"
#include "Items/Pickable.hpp"

using namespace Urho3D;

PickableGold::PickableGold(Context* context) : Pickable(context)
{
}

void PickableGold::set_name(const String& name)
{
    m_name = name;
}

const String& PickableGold::get_name() const
{
    return m_name;
}

void PickableGold::set_description(const String& description)
{
    m_description = description;
}

const String& PickableGold::get_desctiption() const
{
    return m_description;
}

void PickableGold::set_amout(int value)
{
    m_amount = value;
}

const int& PickableGold::get_amout() const
{
    return m_amount;
}
