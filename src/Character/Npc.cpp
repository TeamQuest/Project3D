#include "Character/Npc.hpp"

#include "Character/Components/Moveable.hpp"
#include "Constants.hpp"
#include "Items/Lootable.hpp"
#include "Items/Pickable.hpp"
#include "Utility/InteractionCollider.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/UI.h>

#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>

#pragma clang diagnostic pop

using namespace Urho3D;

Npc::Npc(Context* context) : LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void Npc::Start()
{
    auto cache = GetSubsystem<ResourceCache>();

    auto modelNode = node_->CreateChild("Npc");
    modelNode->SetRotation(Quaternion(180.f, Vector3(0.f, 1.f, 0.f)));
    modelNode->SetPosition(Vector3(Random(40.0f) - 20.0f, 0.0f, Random(40.0f) - 20.0f));
    // modelNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
    modelNode->SetScale(1.f);

    auto* modelObject = modelNode->CreateComponent<AnimatedModel>();
    modelObject->SetModel(cache->GetResource<Model>("Models/Kachujin/Kachujin.mdl"));
    modelObject->SetMaterial(cache->GetResource<Material>("Models/Kachujin/Materials/Kachujin.xml"));
    modelObject->SetCastShadows(true);

    /* animations */
    auto* walkAnimation = cache->GetResource<Animation>("Models/Kachujin/Kachujin_Walk.ani");
    AnimationState* state = modelObject->AddAnimationState(walkAnimation);
    // The state would fail to create (return null) if the animation was not found
    if (state) {
        state->SetWeight(1.0f);
        state->SetLooped(true);
        state->SetTime(Random(walkAnimation->GetLength()));
    }

    auto rigidbody = modelNode->CreateComponent<RigidBody>();
    rigidbody->SetMass(1.f);

    auto collider = modelNode->CreateComponent<CollisionShape>();
    collider->SetBox(Vector3::ONE, Vector3(0.f, 0.5f, 0.f));

    // Create our custom Mover component that will move & animate the model during each frame's update
    modelNode->CreateComponent<Lootable>();

    SubscribeToEvent(node_, E_NODECOLLISION, URHO3D_HANDLER(Npc, handle_collision));
}

void Npc::handle_collision(StringHash /* event_type */, VariantMap& event_data)
{
}

void Npc::Update(float time_step)
{
    // node_->Translate(Vector3::FORWARD * moveSpeed_ * timeStep);
    auto body = node_->GetChild("Npc")->GetComponent<RigidBody>();
    body->ApplyImpulse(node_->GetRotation() * Vector3::ONE * 0.05);

    // If in risk of going outside the plane, rotate the model right
    Vector3 pos = node_->GetPosition();

    // Get the model's first (only) animation state and advance its time. Note the convenience accessor to other components
    // in the same scene node
    if (body->GetLinearVelocity().Length() != 0) {
        auto* model = node_->GetComponent<AnimatedModel>(true);
        if (model->GetNumAnimationStates()) {
            AnimationState* state = model->GetAnimationStates()[0];
            state->AddTime(time_step);
        }
    }
    else {
    }
}

void Npc::set_focused(Node* node)
{
    // auto rigid_body = node->GetComponent<RigidBody>();
    auto x = node_->GetChild("Npc")->GetComponent<Moveable>();
    x->stop();
}