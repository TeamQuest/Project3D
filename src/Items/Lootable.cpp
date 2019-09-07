#include "Items/Lootable.hpp"

#include <algorithm>

using namespace Urho3D;

Lootable::Lootable(Context* context) : LogicComponent(context)
{
}

void Lootable::add_item(SharedPtr<Pickable> item)
{
    m_items.push_back(item);
}

void Lootable::remove_item(Pickable* item)
{
    const auto it = find_if(m_items.begin(), m_items.end(), [&](auto k) { return k.Get() == item; });
    assert(it != m_items.end());
    m_items.erase(it);
}

const std::vector<SharedPtr<Pickable>>& Lootable::get_items()
{
    return m_items;
}
