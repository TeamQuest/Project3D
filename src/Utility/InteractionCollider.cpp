#include "Utility/InteractionCollider.hpp"

#include "Items/Lootable.hpp"
#include "Items/Pickable.hpp"
#include "Utility/Common.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Graphics/Light.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>

#pragma clang diagnostic pop

#include <algorithm>

using namespace Urho3D;

InteractionCollider::InteractionCollider(Context* context) : LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void InteractionCollider::Start()
{
    {
        auto interaction_node = node_->CreateChild("Interaction");
        auto rigidbody = interaction_node->CreateComponent<RigidBody>();
        rigidbody->SetTrigger(true);
        rigidbody->SetKinematic(true);
        rigidbody->SetCollisionLayerAndMask(2, 1);
        auto collider = interaction_node->CreateComponent<CollisionShape>();
        collider->SetBox({0.5f, 2.f, 2.f}, {0.f, 1.f, 1.5f});
    }
    SubscribeToEvent(node_->GetChild("Interaction"), E_NODECOLLISIONEND, [&](auto, VariantMap& event_data) {
        auto node = static_cast<Node*>(event_data[NodeCollisionEnd::P_OTHERNODE].GetPtr());
        if (auto to_remove = node->GetChild("SpotlightOnSelection")) {
            close_window();
            node->RemoveChild(to_remove);
            m_highlighted.Reset();
        }
    });
}

void InteractionCollider::Update(float /* time_step */)
{
    handle_collision();
    handle_interaction();
}

void InteractionCollider::handle_collision()
{
    const auto world = node_->GetScene()->GetComponent<PhysicsWorld>();
    PODVector<RigidBody*> bodies;
    auto collision_body = node_->GetComponent<RigidBody>();
    world->GetCollidingBodies(bodies, node_->GetChild("Interaction")->GetComponent<RigidBody>());
    auto last_rigid_ptr = std::remove_if(bodies.Buffer(), bodies.Buffer() + bodies.Size(), [](RigidBody* rigid) {
        // Remove all non-pickable nodes
        auto node = rigid->GetNode();
        return !node->HasComponent<Pickable>() && !node->HasComponent<Lootable>();
    });
    if (bodies.Empty() || bodies.Buffer() == last_rigid_ptr) {
        return;
    }

    auto closest_body = *std::min_element(bodies.Buffer(), last_rigid_ptr, [&](auto rigid1, auto rigid2) {
        const auto collision_body_pos = collision_body->GetNode()->GetWorldPosition();
        const auto dist1 = (rigid1->GetNode()->GetWorldPosition() - collision_body_pos).LengthSquared();
        const auto dist2 = (rigid2->GetNode()->GetWorldPosition() - collision_body_pos).LengthSquared();
        return dist1 < dist2;
    });
    // If there is a closer body than the highlighted one
    if (closest_body->GetNode() != m_highlighted) {
        if (m_highlighted) {
            m_highlighted->RemoveChild(m_highlighted->GetChild("SpotlightOnSelection"));
            m_highlighted.Reset();
        }
        m_highlighted = closest_body->GetNode();
        // Assign a spotlight
        auto light_node = m_highlighted->CreateChild("SpotlightOnSelection");
        light_node->SetWorldDirection(Vector3::DOWN);
        light_node->SetWorldPosition(closest_body->GetPosition() + Vector3{0.f, 3.f, 0.f});
        auto light = light_node->CreateComponent<Light>();
        light->SetLightType(LightType::LIGHT_SPOT);
        light->SetRange(5.f);
    }
}

Window* InteractionCollider::create_popup_window()
{
    m_window_open = true;
    auto window = *make<Window>(context_)
                       .name("LootWindow")
                       .styleauto()
                       .layout(LM_VERTICAL, 10, IntRect{10, 10, 10, 10})
                       .aligned(HA_LEFT, VA_CENTER)
                       .position(100, 0);
    auto window_title = *make<Text>(context_).styleauto().text("<Lootable object>");
    window->AddChild(window_title);
    if (auto lootable_item = m_highlighted->GetComponent<Lootable>()) {
        const auto anonymous_pro_font = GetSubsystem<ResourceCache>()->GetResource<Font>(("Fonts/Anonymous Pro.ttf"));
        for (auto item : lootable_item->get_items()) {
            auto item_button = *make<Button>(context_).styleauto().minheight(50).minwidth(item->get_name().Length() * 15);
            auto item_text = *make<Text>(context_)
                                  .text(item->get_name())
                                  .font(anonymous_pro_font)
                                  .fontsize(15)
                                  .alignment(HA_CENTER, VA_CENTER)
                                  .textaligned(HA_CENTER);
            item_button->AddChild(item_text);
            window->AddChild(item_button);
        }
    }
    return window;
}

void InteractionCollider::handle_interaction()
{
    if (m_highlighted && GetSubsystem<Input>()->GetKeyPress(KEY_E)) {
        close_window();
        const auto window = create_popup_window();
        GetSubsystem<UI>()->GetRoot()->AddChild(window);
    }
}

void InteractionCollider::close_window()
{
    if (m_window_open) {
        const auto ui_root = GetSubsystem<UI>()->GetRoot();
        ui_root->RemoveChild(ui_root->GetChild("LootWindow", true));
        m_window_open = false;
    }
}
