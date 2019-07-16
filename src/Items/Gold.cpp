#include "Items/Gold.hpp"
#include "Items/Pickable.hpp"

using namespace Urho3D;

Gold::Gold(Context* context) : Pickable(context)
{
}

void Gold::set_amout(int value)
{
    m_amount = value;
}

const int& Gold::get_amout() const
{
    return m_amount;
}
