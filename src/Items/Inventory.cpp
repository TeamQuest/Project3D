#include "Items/Inventory.hpp"

#include "Scenes/Scenes.hpp"
#include "Utility/Common.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

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
#include <Urho3D/Scene/Scene.h>

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
        auto item = get<Pickable>(event_data[ItemClickedEvent::P_ITEM]);
        URHO3D_LOGINFOF("Dodaje do inv: %p", item);
    });
    {
        m_window = *make<Window>(context_)
                .name("InventoryWindow")
                .styleauto()
                .layout(LM_VERTICAL, 10, IntRect{10, 10, 10, 10})
                .minsize(300, 300)
                .aligned(HorizontalAlignment::HA_LEFT, VerticalAlignment::VA_CENTER)
                .position(100, 0);
        m_window->SetEnabledRecursive(false);
        m_window->SetVisible(false);
        GetSubsystem<UI>()->GetRoot()->AddChild(m_window);
    }
    {
        m_window_description = *make<Window>(context_)
                .name("DescriptionWindow")
                .styleauto()
                .minsize(300, 300)
                .layout(LM_VERTICAL, 10, IntRect{10, 10, 10, 10})
                .aligned(HA_LEFT, VA_CENTER)
                .position(450, 0);
        m_window_description->SetEnabledRecursive(false);
        m_window_description->SetVisible(false);
        GetSubsystem<UI>()->GetRoot()->AddChild(m_window_description);
    }
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Inventory, toggle_key_down));
    SubscribeToEvent(E_OPEN_INVENTORY, URHO3D_HANDLER(Inventory, toggle) );
}

void Inventory::Update(float /* time_step */)
{
}

bool Inventory::add(const SharedPtr<Pickable>& pickable)
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

void Inventory::toggle_key_down(StringHash /* event_type */, VariantMap & /* event_data */) {

    const auto input = GetSubsystem<Input>();
    if (!input->GetKeyPress(KEY_I)) {
        return;
    }

    SendEvent(E_OPEN_INVENTORY);
}

void Inventory::toggle(StringHash /* event_type */, VariantMap& /* event_data */)
{
    if (m_window->IsEnabled()) {
        URHO3D_LOGINFO("Closing inventory...");
        const auto loot_window = GetSubsystem<UI>()->GetRoot()->GetChild("InventoryWindow", false);
        loot_window->RemoveAllChildren();
        m_window->SetEnabledRecursive(false);
        m_window->SetVisible(false);
    }
    else {
        URHO3D_LOGINFO("Opening inventory...");
        m_window->SetEnabled(true);
        m_window->SetVisible(true);
        const auto anonymous_pro_font = GetSubsystem<ResourceCache>()->GetResource<Font>(("Fonts/Anonymous Pro.ttf"));

        auto title_text = *make<Text>(context_)
                              .text("okienko")
                              .font(anonymous_pro_font)
                              .fontsize(20)
                              //   .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER)
                              .textaligned(HA_CENTER);

        m_window->AddChild(title_text);
        for (const auto& item : m_items) {
            auto item_button = *make<Button>(context_)
                                    .styleauto()
                                    // .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER)
                                    .minwidth(200)
                                    .fixedheight(50)
                                    .var("item", item.Get());

            auto item_text = *make<Text>(context_)
                                  .text(item->get_name())
                                  .font(anonymous_pro_font)
                                  .fontsize(20)
                                  //   .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER)
                                  .textaligned(HA_CENTER);

            // Opening item description
            SubscribeToEvent(item_button, E_RELEASED, [this](auto, auto& event_data) {
                auto button = static_cast<Button*>(event_data[Released::P_ELEMENT].GetPtr());
                auto item = get<Pickable>(button->GetVar("item"));

                if (m_window_description->IsEnabled()) {
                    URHO3D_LOGINFO("Closing item description...");
                    const auto loot_window = GetSubsystem<UI>()->GetRoot()->GetChild("DescriptionWindow", false);
                    loot_window->RemoveAllChildren();
                    m_window_description->SetEnabledRecursive(false);
                    m_window_description->SetVisible(false);
                } else {
                    URHO3D_LOGINFO("Opening item description...");
                    m_window_description->SetEnabled(true);
                    m_window_description->SetVisible(true);

                    auto item_text = *make<Text>(context_)
                            .text("item description")
                            .font(GetSubsystem<ResourceCache>()->GetResource<Font>(("Fonts/Anonymous Pro.ttf")))
                            .fontsize(15)
                            .textaligned(HA_CENTER);
                    m_window_description->AddChild(item_text);

                    auto item_description = *make<Text>(context_)
                            .text(item->get_description())
                            .font(GetSubsystem<ResourceCache>()->GetResource<Font>(("Fonts/Anonymous Pro.ttf")))
                            .fontsize(20)
                            .textaligned(HA_CENTER);
                    m_window_description->AddChild(item_description);
                }
            });

            item_button->AddChild(item_text);
            m_window->AddChild(item_button);
        }
    }
}

bool Inventory::is_full() const
{
    return m_items.size() >= m_capacity;
}
