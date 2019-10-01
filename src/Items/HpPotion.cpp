#include "Items/HpPotion.hpp"

using namespace Urho3D;

HpPotion::HpPotion(Context* context)
        :Pickable(context)
{
}

float HpPotion::get_hp_points_to_restore() const
{
    return m_hp_points;
}
