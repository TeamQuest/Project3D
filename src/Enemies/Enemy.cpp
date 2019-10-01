#include "Enemies/Enemy.hpp"

#include "Character/Status.hpp"
#include "Constants.hpp"
#include "Utility/Common.hpp"

#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Physics/CollisionShape.h>

using namespace Urho3D;

Enemy::Enemy(Context* context) : LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void Enemy::Start()
{
    auto anim_ctrl = node_->GetComponent<AnimationController>(true);
    anim_ctrl->PlayExclusive("Models/NinjaSnowWar/Ninja_Walk.ani", 0, false, 0.2);

    node_->SetRotation(Quaternion(140.f, Vector3::UP));

    auto rigidbody = node_->GetComponent<RigidBody>();
    rigidbody->SetMass(1.f);
    rigidbody->SetAngularFactor(Vector3::ZERO);

    auto collider = node_->CreateComponent<CollisionShape>();
    collider->SetCapsule(0.7f, 1.8f, Vector3::UP * 0.9f);
}

void Enemy::Update(float time_step)
{
    if (m_target) {
        m_move_speed = 1.f;
        node_->LookAt(m_target->GetPosition());
        {
            auto position = node_->GetPosition();
            auto dist = (m_target->GetPosition() - position).Length();
            if (dist < 1.8f) {
                m_move_speed = 0.f;
            }
        }
    }
    auto rigid = GetComponent<RigidBody>();
    rigid->ApplyImpulse(node_->GetRotation() * Vector3::FORWARD * m_move_speed * time_step * 20.f);
    auto velocity_xz = [&] {
        auto&& v = rigid->GetLinearVelocity();
        v.y_ = 0;
        return v;
    }();
    rigid->ApplyImpulse(-velocity_xz * BRAKE_FORCE);

    auto animator = node_->GetComponent<AnimationController>(true);
    if (rigid->GetLinearVelocity().LengthSquared() > 0.0001f) {
        animator->PlayExclusive("Models/NinjaSnowWar/Ninja_Walk.ani", 0, true, 0.2f);
    }
    else {
        if (animator->IsPlaying(3)) {
            during_attack_anim = false;
        }
        if (!during_attack_anim) {
            auto attack_style = Random(0, 5);
            String style;
            switch (attack_style) {
                case 0:
                    style = "SideKick";
                    break;
                case 1:
                    style = "Kick";
                    break;
                case 2:
                    style = "Spin";
                    break;
                case 3:
                    style = "Attack1";
                    break;
                case 4:
                    style = "Attack2";
                    break;
                case 5:
                    style = "Attack3";
                    break;
                default:
                    break;
            }
            animator->PlayExclusive("Models/NinjaSnowWar/Ninja_" + style + ".ani", 3, false, 0.2f);
            during_attack_anim = true;
            auto player = GetScene()->GetChild(PLAYER_NAME);
            player->GetComponent<RigidBody>()->ApplyForce(node_->GetRotation() * (0.3f * Vector3::UP + Vector3::FORWARD) * 90.f);
            if (Random(1.0f) > 0.5f) {
                player->GetComponent<Status>()->set_hp_points(player->GetComponent<Status>()->get_hp_points() - 2.5f);
            }
            else {
                URHO3D_LOGWARNING("MISS");
            }
        }
    }
}

void Enemy::set_hp_points(float points) {
    m_hp_points = points;
}

float Enemy::get_hp_points() {
    return m_hp_points;
}

void Enemy::assign_target(Node* target) {
    m_target = target;
}
