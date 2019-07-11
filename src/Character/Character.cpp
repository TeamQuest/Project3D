#include "Character/Character.hpp"

#include "Constants.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/IO/MemoryBuffer.h>

#include <Urho3D/Input/Input.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/UI.h>

#pragma clang diagnostic pop

#include <tuple>

using namespace Urho3D;

Character::Character(Context* context) : LogicComponent(context), m_on_ground(false), m_can_jump(true), m_time_in_air(0.0f)
{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
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
    // Component has been inserted into its scene node. Subscribe to events now
    SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(Character, handle_collision));
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

        //////// Somebody explain this bit
        if (m_controls.IsDown(MovementKey::JUMP)) {
            if (m_can_jump) {
                body->ApplyImpulse(Vector3::UP * JUMP_FORCE);
                m_can_jump = false;
                animator->PlayExclusive("Models/Mutant/Mutant_Jump1.ani", 0, false, 0.2f);
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

    // Add character yaw & pitch from the mouse motion
    const auto [mouse_x, mouse_y] = input->GetMouseMove();
    m_controls.yaw_ += static_cast<float>(mouse_x) * YAW_SENSITIVITY;
    m_controls.pitch_ += static_cast<float>(mouse_y) * YAW_SENSITIVITY;

    // Limit pitch
    m_controls.pitch_ = Clamp(m_controls.pitch_, -80.0f, 80.0f);
    // Set rotation already here so that it's updated every rendering frame instead of every physics frame
    GetNode()->SetRotation(Quaternion(m_controls.yaw_, Vector3::UP));
}

void Character::handle_camera(SharedPtr<Node> camera, PhysicsWorld* world)
{
    auto&& rotation = GetNode()->GetRotation();
    const auto dir = rotation * Quaternion(m_controls.pitch_, Vector3::RIGHT);
    //////// Somebody explain why aim_point is calculated like this
    // Third person camera: position behind the character
    const auto aim_point = GetNode()->GetPosition() + rotation * Vector3::UP * 1.7f;

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

        //////// Commented because this doesn't seem to be needed
        // If contact is below node center and pointing up, assume it's a ground contact
        // if (contact_position.y_ < node_->GetPosition().y_ + 1.f) {
        auto&& level = contact_normal.y_;
        if (level > 0.75f) {
            m_on_ground = true;
        }
        // }
    }
}
