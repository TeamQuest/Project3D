#include "Utility/InteractionCollider.hpp"

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
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>

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
        collider->SetBox({2.f, 2.f, 2.f}, {0.f, 1.f, 1.5f});
    }
    SubscribeToEvent(node_->GetChild("Interaction"), E_NODECOLLISIONEND, [&](auto, VariantMap& event_data) {
        auto node = static_cast<Node*>(event_data[NodeCollisionEnd::P_OTHERNODE].GetPtr());
        if (auto to_remove = node->GetChild("SpotlightOnSelection")) {
            node->RemoveChild(to_remove);
            m_highlighted.Reset();
        }
    });
}

void InteractionCollider::Update(float /* time_step */)
{
    handle_collision();
}

void InteractionCollider::handle_collision()
{
    const auto world = node_->GetScene()->GetComponent<PhysicsWorld>();
    PODVector<RigidBody*> bodies;
    auto collision_body = node_->GetComponent<RigidBody>();
    world->GetCollidingBodies(bodies, node_->GetChild("Interaction")->GetComponent<RigidBody>());
    if (bodies.Empty()) {
        return;
    }
    auto last_rigid_ptr = std::remove_if(bodies.Buffer(), bodies.Buffer() + bodies.Size(), [](RigidBody* rigid) {
        // Remove all non-pickable nodes
        return !rigid->GetNode()->HasComponent<Pickable>();
    });

    auto closest_body = *std::min_element(bodies.Buffer(), last_rigid_ptr, [&](auto rigid1, auto rigid2) {
        const auto dist1 = (rigid1->GetNode()->GetWorldPosition() - collision_body->GetNode()->GetWorldPosition()).LengthSquared();
        const auto dist2 = (rigid2->GetNode()->GetWorldPosition() - collision_body->GetNode()->GetWorldPosition()).LengthSquared();
        return dist1 < dist2;
    });
    // auto pick = closest_body->GetComponent<Pickable>();
    // URHO3D_LOGWARNING("Found a pickable item: " + pick->item());
    if (!closest_body->GetNode()->GetChild("SpotlightOnSelection")) {
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
