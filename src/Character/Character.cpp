#include "Character/Character.hpp"

#include "Constants.hpp"
#include "Items/Inventory.hpp"
#include "Items/Pickable.hpp"
#include "Status.hpp"
#include "Utility/InteractionCollider.hpp"
#include "Quests/QuestRunner.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimationController.h>
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
#include <Utility/Common.hpp>

#pragma clang diagnostic pop

using namespace Urho3D;

Character::Character(Context* context) : LogicComponent(context)
{
    SetUpdateEventMask(USE_FIXEDUPDATE);
}

// void Character::register_object(Context* context)
// {
//     context->RegisterFactory<Character>();
//     // These macros register the class attributes to the Context for automatic load / save handling.
//     // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
//     URHO3D_ATTRIBUTE("Controls Yaw", float, m_controls.yaw_, 0.0f, AM_DEFAULT);
//     URHO3D_ATTRIBUTE("Controls Pitch", float, m_controls.pitch_, 0.0f, AM_DEFAULT);
//     URHO3D_ATTRIBUTE("On Ground", bool, m_on_ground, false, AM_DEFAULT);
//     URHO3D_ATTRIBUTE("OK To Jump", bool, m_can_jump, true, AM_DEFAULT);
//     URHO3D_ATTRIBUTE("In Air Timer", float, m_time_in_air, 0.0f, AM_DEFAULT);
// }

void Character::Start()
{
    auto cache = GetSubsystem<ResourceCache>();

    auto asset_node = node_->CreateChild("Character Asset Node");
    asset_node->SetRotation(Quaternion(180.f, Vector3(0.f, 1.f, 0.f)));

    // Create the rendering component + animation controller
    auto model = asset_node->CreateComponent<AnimatedModel>();
    model->SetModel(cache->GetResource<Model>("Models/Mutant/Mutant.mdl"));
    model->SetMaterial(cache->GetResource<Material>("Models/Mutant/Materials/mutant_M.xml"));
    model->SetCastShadows(true);
    asset_node->CreateComponent<AnimationController>();

    // Set the head bone for manual control
    model->GetSkeleton().GetBone("Mutant:Head")->animated_ = false;

    // Create rigidbody, and set non-zero mass so that the body becomes dynamic
    auto body = node_->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);
    body->SetMass(1.0f);

    // Set zero angular factor so that physics doesn't turn the character on its own.
    // Instead we will control the character yaw manually
    body->SetAngularFactor(Vector3::ZERO);

    // Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
    body->SetCollisionEventMode(CollisionEventMode::COLLISION_ALWAYS);

    // Set a capsule shape for collision
    auto shape = node_->CreateComponent<CollisionShape>();
    shape->SetCapsule(0.7f, 1.8f, Vector3(0.0f, 0.9f, 0.0f));

    // Set an interaction component
    node_->CreateComponent<InteractionCollider>();

    // Contain items in inventory
    node_->CreateComponent<Inventory>();

    // Allow starting quests from Quest Givers
    node_->CreateComponent<QuestRunner>();

    // Status component
    auto component_status = node_->CreateComponent<Status>();
    component_status->set_hp_points(45);
    component_status->set_character_name(GetGlobalVar("PlayerName").GetString());

    // Component has been inserted into its scene node. Subscribe to events now
    SubscribeToEvent(node_, E_NODECOLLISION, URHO3D_HANDLER(Character, handle_collision));
}

void Character::FixedUpdate(float time_step)
{
    // TODO: Could cache the components for faster access instead of finding them each frame
    auto body = GetComponent<RigidBody>();
    auto animator = node_->GetComponent<AnimationController>(true);

    // Update the in air timer. Reset if grounded
    m_time_in_air = !m_on_ground ? m_time_in_air + time_step : 0.f;
    // When character has been in air less than 1/10 second, it's still interpreted as being on ground
    auto soft_grounded = m_time_in_air < INAIR_THRESHOLD_TIME;

    // Update movement & animation
    auto move_dir = Vector3::ZERO;

    if (m_controls.IsDown(MovementKey::FORWARD)) {
        move_dir += Vector3::FORWARD;
    }
    if (m_controls.IsDown(MovementKey::BACK)) {
        move_dir += Vector3::BACK;
    }
    if (m_controls.IsDown(MovementKey::LEFT)) {
        move_dir += Vector3::LEFT;
    }
    if (m_controls.IsDown(MovementKey::RIGHT)) {
        move_dir += Vector3::RIGHT;
    }

    // Normalize move vector so that diagonal strafing is not faster
    move_dir.Normalize();

    // If in air, allow control, but slower than when on ground
    body->ApplyImpulse(node_->GetRotation() * move_dir * (soft_grounded ? MOVE_FORCE : INAIR_MOVE_FORCE));

    auto velocity_xz = [&] {
        auto&& v = body->GetLinearVelocity();
        v.y_ = 0;
        return v;
    }();
    if (soft_grounded) {
        // When on ground, apply a braking force to limit maximum ground velocity
        body->ApplyImpulse(-velocity_xz * BRAKE_FORCE);

        if (m_controls.IsDown(MovementKey::JUMP)) {
            if (m_can_jump) {
                body->ApplyImpulse(Vector3::UP * JUMP_FORCE);
                m_can_jump = false;
                animator->PlayExclusive("Models/Mutant/Mutant_Run.ani", 0, false, 0.2f);
            }
        }
        else {
            m_can_jump = true;
        }
    }

    if (!m_on_ground) {
        animator->PlayExclusive("Models/Mutant/Mutant_Jump1.ani", 0, false, 0.2f);
    }
    else {
        // Play walk animation if moving on ground, otherwise fade it out
        if (soft_grounded && move_dir != Vector3::ZERO) {
            animator->PlayExclusive("Models/Mutant/Mutant_Run.ani", 0, true, 0.2f);
        } else if (m_controls.IsDown(MovementKey::PUNCH)) {
            animator->PlayExclusive("Models/Mutant/Mutant_Punch.ani", 0, true, 0.2f);
        } else if (m_controls.IsDown(MovementKey::SWIPE)) {
            animator->PlayExclusive("Models/Mutant/Mutant_Swipe.ani", 0, true, 0.2f);
        } else if (m_controls.IsDown(MovementKey::KICK)) {
            animator->PlayExclusive("Models/Mutant/Mutant_Kick.ani", 0, true, 0.2f);
        }
        else {
            animator->PlayExclusive("Models/Mutant/Mutant_Idle0.ani", 0, true, 0.2f);
        }

        // Set walk animation speed proportional to velocity
        animator->SetSpeed("Models/Mutant/Mutant_Run.ani", velocity_xz.Length() * 0.3f);
    }
    m_on_ground = false;
}

void Character::handle_movement()
{
    // Clear previous controls
    m_controls.Set(MovementKey::RESET, false);

    // Update controls using keys
    auto input = GetSubsystem<Input>();
    m_controls.Set(MovementKey::FORWARD, input->GetKeyDown(KEY_W));
    m_controls.Set(MovementKey::BACK, input->GetKeyDown(KEY_S));
    m_controls.Set(MovementKey::LEFT, input->GetKeyDown(KEY_A));
    m_controls.Set(MovementKey::RIGHT, input->GetKeyDown(KEY_D));
    m_controls.Set(MovementKey::JUMP, input->GetKeyDown(KEY_SPACE));
    m_controls.Set(MovementKey::PUNCH, input->GetKeyDown(KEY_P));
    m_controls.Set(MovementKey::SWIPE, input->GetKeyDown(KEY_L));
    m_controls.Set(MovementKey::KICK, input->GetKeyDown(KEY_K));

    // Add character yaw & pitch from the mouse motion

    // input->SetMouseVisible(true);
    if (input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT)) {
        // input->SetMouseVisible(false);
        const auto [mouse_x, mouse_y] = input->GetMouseMove();
        m_controls.yaw_ += static_cast<float>(mouse_x) * YAW_SENSITIVITY;
        m_controls.pitch_ += static_cast<float>(mouse_y) * PITCH_SENSITIVITY;
        // Limit pitch
        m_controls.pitch_ = Clamp(m_controls.pitch_, -80.0f, 80.0f);
        // Set rotation already here so that it's updated every rendering frame instead of every physics frame
        node_->SetRotation(Quaternion(m_controls.yaw_, Vector3::UP));
    }
}

void Character::handle_camera(SharedPtr<Node> camera, PhysicsWorld* world)
{
    auto&& rotation = node_->GetRotation();
    const auto dir = rotation * Quaternion(m_controls.pitch_, Vector3::RIGHT);
    // Third person camera: position behind the character
    const auto aim_point = node_->GetPosition() + rotation * Vector3::UP * 1.7f;
    // Collide camera ray with static physics objects (layer bitmask 2) to ensure we see the character properly
    const auto ray_dir = dir * Vector3::BACK;
    auto ray_distance = CAMERA_INITIAL_DIST;
    PhysicsRaycastResult raycast;
    world->RaycastSingle(raycast, Ray(aim_point, ray_dir), ray_distance, 2);
    if (raycast.body_) {
        ray_distance = Min(ray_distance, raycast.distance_);
    }
    ray_distance = Clamp(ray_distance, CAMERA_MIN_DIST, CAMERA_MAX_DIST);

    camera->SetPosition(aim_point + ray_dir * ray_distance);
    camera->SetRotation(dir);
}

void Character::handle_collision(StringHash /* event_type */, VariantMap& event_data)
{
    // Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
    auto contacts = MemoryBuffer(event_data[NodeCollision::P_CONTACTS].GetBuffer());

    while (!contacts.IsEof()) {
        /* auto contact_position = */ contacts.ReadVector3();
        auto contact_normal = contacts.ReadVector3();
        /* auto contact_distance = */ contacts.ReadFloat();
        /* auto contact_impulse = */ contacts.ReadFloat();
        if (contact_normal.y_ > 0.75f) {
            m_on_ground = true;
        }
    }
}

void Character::adjust_head_pitch()
{
    // Sets head pitch based on camera

    const auto rotation = node_->GetRotation();
    const auto head = node_->GetChild("Mutant:Head", true);
    const auto pitch_constraint = Clamp(m_controls.pitch_, -45.0f, 45.0f);
    // Rotate head along x-axis by the pitch angle
    const auto head_dir = node_->GetRotation() * Quaternion(pitch_constraint, Vector3::RIGHT);
    const auto target = head->GetWorldPosition() + head_dir * Vector3::BACK;
    head->LookAt(target, Vector3::UP);
}
