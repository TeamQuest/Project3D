#include "Utility/InteractionCollider.hpp"

#include "Items/Lootable.hpp"
#include "Items/Pickable.hpp"

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
        auto&& rigidbody = interaction_node->CreateComponent<RigidBody>();
        rigidbody->SetTrigger(true);
        rigidbody->SetKinematic(true);
        rigidbody->SetCollisionLayerAndMask(2, 1);
        auto&& collider = interaction_node->CreateComponent<CollisionShape>();
        collider->SetBox({1.f, 2.f, 2.f}, {0.f, 1.f, 1.5f});
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

void InteractionCollider::handle_interaction()
{
    if (m_highlighted && GetSubsystem<Input>()->GetKeyPress(KEY_E)) {
        close_window();
        m_window_open = true;
        auto ui_root = GetSubsystem<UI>()->GetRoot();
        auto window = new Window(context_);
        window->SetName("LootWindow");
        window->SetStyleAuto();
        window->SetLayout(LM_VERTICAL, 10, {10, 10, 10, 10});
        window->SetAlignment(HA_LEFT, VA_CENTER);
        window->SetPosition(100, 0);
        ui_root->AddChild(window);

        auto window_title = new Text(context_);
        window_title->SetName("LootWindowTitle");
        window_title->SetStyleAuto();
        window_title->SetText("<Lootable object>");
        window->AddChild(window_title);

        if (auto lootable_item = m_highlighted->GetComponent<Lootable>()) {
            for (auto item : lootable_item->get_items()) {
                auto item_button = new Button(context_);
                item_button->SetStyleAuto();
                item_button->SetMinHeight(50);
                auto item_text = new Text(context_);
                const auto cache = GetSubsystem<ResourceCache>();
                item_text->SetText(item->name());
                item_text->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"));
                item_text->SetFontSize(15);
                item_text->SetAlignment(HA_CENTER, VA_CENTER);
                item_text->SetTextAlignment(HA_CENTER);
                item_button->AddChild(item_text);
                item_button->SetMinWidth(item->name().Length() * 15);
                window->AddChild(item_button);
            }
        }
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