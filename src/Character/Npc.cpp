#include "Character/Npc.hpp"

#include "Constants.hpp"
#include "Items/Lootable.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SmoothedTransform.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Resource/ResourceCache.h>
#pragma clang diagnostic pop

using namespace Urho3D;

Npc::Npc(Context* context) : LogicComponent(context), move_speed(1.f)
{
    SetUpdateEventMask(USE_UPDATE);
}

void Npc::Start()
{
    auto cache = GetSubsystem<ResourceCache>();

    auto modelNode = node_->CreateChild("Npc");
    node_->SetRotation(Quaternion(0.f, Vector3::UP));
    node_->SetScale(1.f);

    auto model = modelNode->CreateComponent<AnimatedModel>();
    model->SetModel(cache->GetResource<Model>("Models/Kachujin/Kachujin.mdl"));
    model->SetMaterial(cache->GetResource<Material>("Models/Kachujin/Materials/Kachujin.xml"));
    model->SetCastShadows(true);

    /* animations */
    auto walk_animation = cache->GetResource<Animation>("Models/Kachujin/Kachujin_Walk.ani");
    auto state = model->AddAnimationState(walk_animation);
    if (state) {
        state->SetWeight(1.0f);
        state->SetLooped(true);
        state->SetTime(Random(walk_animation->GetLength()));
    }

    auto rigidbody = node_->CreateComponent<RigidBody>();
    rigidbody->SetMass(1.f);
    rigidbody->SetAngularFactor(Vector3::ZERO);

    auto collider = node_->CreateComponent<CollisionShape>();
    collider->SetCapsule(0.7f, 1.8f, Vector3::UP * 0.9);

    node_->CreateComponent<Lootable>();
    node_->CreateComponent<SmoothedTransform>();

    SubscribeToEvent(node_, E_NODECOLLISION, URHO3D_HANDLER(Npc, handle_collision));
}

void Npc::handle_collision(StringHash /* event_type */, VariantMap& /* event_data */)
{
}

void Npc::correct_speed()
{
    auto position = node_->GetPosition();
    if (target) {
        auto dist = (target->GetPosition() - position).Length();
        if(dist < 1.8f) {
            stop_walking();
            target = nullptr;
        }
    }
}

void Npc::Update(float time_step)
{
    if (target) {
        node_->LookAt(target->GetPosition());
        correct_speed();
    }
    auto rigid = GetComponent<RigidBody>();
    rigid->ApplyImpulse(node_->GetRotation() * Vector3::FORWARD * move_speed * time_step * 20.f);
    auto velocity_xz = [&] {
        auto&& v = rigid->GetLinearVelocity();
        v.y_ = 0;
        return v;
    }();
    rigid->ApplyImpulse(-velocity_xz * BRAKE_FORCE);

    auto model = node_->GetComponent<AnimatedModel>(true);
    if (move_speed != 0) {
        if (model->GetNumAnimationStates()) {
            auto state = model->GetAnimationStates()[0];
            state->AddTime(time_step);
        }
    }
    else {
        if (model->GetNumAnimationStates()) {
            auto state = model->GetAnimationStates()[0];
            state->SetTime(1.8f);
        }
    }
}

void Npc::stop_walking()
{
    move_speed = 0;
    saved_rotation = node_->GetRotation();

    node_->LookAt(GetScene()->GetChild("jack")->GetPosition());
    auto target_rotation = node_->GetRotation();

    auto smoothed_transform = node_->GetComponent<SmoothedTransform>();
    smoothed_transform->SetTargetRotation(target_rotation);
}

void Npc::resume()
{
    move_speed = 1.f;

    auto smoothed_transform = node_->GetComponent<SmoothedTransform>();
    smoothed_transform->SetTargetRotation(saved_rotation);
    node_->SetRotation(saved_rotation);
}

bool Npc::focused()
{
    return move_speed == 0;
}


void Npc::follow(Node* new_target)
{
    target = new_target;
    move_speed = 1.f;
}
