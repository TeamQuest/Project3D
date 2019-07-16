#include "Utility/InteractionCollider.hpp"

#include "Items/Pickable.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>

#pragma clang diagnostic pop

using namespace Urho3D;

InteractionCollider::InteractionCollider(Context* context) : LogicComponent(context)
{
    SetUpdateEventMask(USE_FIXEDPOSTUPDATE);
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
    SubscribeToEvent(node_->GetChild("Interaction"), E_NODECOLLISION, URHO3D_HANDLER(InteractionCollider, handle_collision));
}

void InteractionCollider::handle_collision(StringHash /* event_type */, VariantMap& event_data)
{
    const auto input = GetSubsystem<Input>();
    if (input->GetKeyPress(KEY_E)) {
        auto node_collider = static_cast<Node*>(event_data[NodeCollision::P_OTHERNODE].GetPtr());

        if (auto pick = node_collider->GetComponent<Pickable>()) {
            URHO3D_LOGWARNING("Found a pickable item: " + pick->item());
        }
    }
}
