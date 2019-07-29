#include "Items/Inventory.hpp"
#include "Scenes/Scenes.hpp"
#include "Utility/Common.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>

#pragma clang diagnostic pop

using namespace Urho3D;

Inventory::Inventory(Context* context) : LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void Inventory::Start()
{
    // code
    SubscribeToEvent(E_ITEM_CLICKED, [&](auto, VariantMap& event_data) {
        //
        auto item = static_cast<Pickable*>(event_data[ItemClickedEvent::P_ITEM].GetPtr());
        URHO3D_LOGINFOF("Dodaje do inv: %p", item);
    });

    m_window = *make<Window>(context_)
                    .name("InventoryWindow")
                    .styleauto()
                    .aligned(HorizontalAlignment::HA_LEFT, VerticalAlignment::VA_CENTER)
                    .position(100, 0);
    m_window->SetEnabledRecursive(false);
    m_window->SetVisible(false);
    GetSubsystem<UI>()->GetRoot()->AddChild(m_window);

    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Inventory, toggle));
}

void Inventory::Update(float /* time_step */)
{
}

bool Inventory::add(Pickable* pickable)
{
    if (is_full()) {
        return false;
    }
    m_items.push_back(pickable);
    URHO3D_LOGINFO("Item " + pickable->get_name() + " added to inventory");
    return true;
}

void Inventory::remove(Pickable* pickable)
{
    m_items.erase(find(begin(m_items), end(m_items), pickable));
    URHO3D_LOGINFO("Item " + pickable->get_name() + " removed from inventory");
}

void Inventory::toggle(StringHash /* event_type */, VariantMap& /* event_data */)
{
    const auto input = GetSubsystem<Input>();
    if (!input->GetKeyPress(KEY_I)) {
        return;
    }
    if (m_window->IsEnabled()) {
        URHO3D_LOGINFO("Closing inventory...");
        const auto loot_window = GetSubsystem<UI>()->GetRoot()->GetChild("InventoryWindow", false);
        loot_window->RemoveAllChildren();
        m_window->SetEnabledRecursive(false);
        m_window->SetVisible(false);
        m_opened = false;
    }
    else {
        URHO3D_LOGINFO("Opening inventory...");
        const auto anonymous_pro_font = GetSubsystem<ResourceCache>()->GetResource<Font>(("Fonts/Anonymous Pro.ttf"));
        auto item_button = *make<Button>(context_).styleauto().size(200, 200).alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER);

        auto item_text = *make<Text>(context_)
                              .text("okienko")
                              .font(anonymous_pro_font)
                              .fontsize(20)
                              .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER)
                              .textaligned(HA_CENTER);

        item_button->AddChild(item_text);
        m_window->AddChild(item_button);
        m_opened = true;
    }
}

bool Inventory::is_full() const
{
    return m_items.size() >= m_capacity;
}