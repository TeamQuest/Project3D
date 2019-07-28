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

void Lootable::remove_item(SharedPtr<Pickable> item)
{
    m_items.erase(find(begin(m_items), end(m_items), item));
}

const std::vector<SharedPtr<Pickable>>& Lootable::get_items()
{
    return m_items;
}
