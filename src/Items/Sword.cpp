#include "Items/Sword.hpp"

using namespace Urho3D;

Sword::Sword(Context* context) : Pickable(context)
{
}

void Sword::set_dmg(int value)
{
    m_dmg = value;
}

const int& Sword::get_dmg() const
{
    return m_dmg;
}
