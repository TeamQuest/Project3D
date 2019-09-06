#include "Character/Npc.hpp"

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
#include <Urho3D/Scene/SmoothedTransform.h>
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

Npc::Npc(Context* context) : LogicComponent(context), move_speed(1.f), rotation_speed(Random(0.f,0.5f))
{
    SetUpdateEventMask(USE_UPDATE);
    direction = Urho3D::Vector3::ONE;
    target_possition = Urho3D::Vector3::ZERO;
    go_to_flag = false;
}

void Npc::Start()
{
    auto cache = GetSubsystem<ResourceCache>();

    auto modelNode = node_->CreateChild("Npc");
    node_->SetRotation(Quaternion(180.f, Vector3(0.f, 1.f, 0.f)));
    node_->SetPosition(Vector3(Random(40.0f) - 20.0f, 0.0f, Random(40.0f) - 20.0f));
    // modelNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
    node_->SetScale(1.f);

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

    auto rigidbody = node_->CreateComponent<RigidBody>();
    rigidbody->SetMass(1.f);

    auto collider = node_->CreateComponent<CollisionShape>();
    collider->SetBox(Vector3::ONE, Vector3(0.f, 0.5f, 0.f));

    node_->CreateComponent<Lootable>();
    node_->CreateComponent<SmoothedTransform>();

    SubscribeToEvent(node_, E_NODECOLLISION, URHO3D_HANDLER(Npc, handle_collision));
}

void Npc::handle_collision(StringHash /* event_type */, VariantMap& event_data)
{
}

void Npc::correct_direction()
{   
     auto possition = node_->GetPosition();
     direction = (possition - target_possition).Abs().Normalized();

}
void Npc::correct_speed()
{   
    auto possition = node_->GetPosition();
    auto l_direction = possition - target_possition;

    move_speed = Min(l_direction.Length()/3.f, 3.f);

    //URHO3D_LOGINFO(String(move_speed));

    if(move_speed < 1.8f)
    {
        move_speed = 0.f;
        go_to_flag = false;
        rotation_speed = 0.f;
    }
}

void Npc::Update(float time_step)
{
    //auto l_direction = direction.Abs().Normalized();
    node_->Translate(Vector3::FORWARD * move_speed * direction * time_step);

    if(go_to_flag)
    {
        correct_direction();
        correct_speed();
    }
    

    if(focused()) return;
    node_->Yaw(rotation_speed);

    auto body = node_->GetComponent<RigidBody>();

    if (body->GetLinearVelocity().Length() != 0) {
        auto* model = node_->GetComponent<AnimatedModel>(true);
        if (model->GetNumAnimationStates()) {
            AnimationState* state = model->GetAnimationStates()[0];
            state->AddTime(time_step);
        }
    }
}

void Npc::stop(const Vector3 & target)
{
    move_speed = 0;

    saved_rotation = node_->GetRotation();
    node_->LookAt(target);

    auto target_rotation = node_->GetRotation();
    auto smoothed_transform = node_->GetComponent<SmoothedTransform>();

    smoothed_transform->SetTargetRotation(target_rotation);
}

void Npc::resume()
{
    move_speed = 1.f;
    rotation_speed = 1.f;
    direction = Urho3D::Vector3::ONE;

    auto smoothed_transform = node_->GetComponent<SmoothedTransform>();
    smoothed_transform->SetTargetRotation(saved_rotation);
    //node_->SetRotation(saved_rotation);
}

bool Npc::focused()
{
    return move_speed == 0;
}


void Npc::go_to(const Urho3D::Vector3 &target)
{
    target_possition = target;
    go_to_flag = true;
    
}