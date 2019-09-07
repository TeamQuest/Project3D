#include "Enemies/Enemy.hpp"

#include "Constants.hpp"
#include "Quests/QuestRunner.hpp"
#include "Character/Npc.hpp"
#include "Utility/Common.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>

#pragma clang diagnostic pop

using namespace Urho3D;

Enemy::Enemy(Context* context) : LogicComponent(context)
{
}

void Enemy::Start()
{
    auto cache = GetSubsystem<ResourceCache>();

    node_->CreateComponent<Npc>();
    node_->RemoveComponent(node_->GetComponent<AnimatedModel>());

    auto model = node_->CreateComponent<AnimatedModel>();
    model->SetModel(cache->GetResource<Model>("Models/NinjaSnowWar/Ninja.mdl"));
    model->SetMaterial(cache->GetResource<Material>("Materials/NinjaSnowWar/Ninja.xml"));
    model->SetCastShadows(true);
}

void Enemy::Update(float /* time_step */)
{

}

void Enemy::set_hp_points(float points) {
    m_hp_points = points;
}

float Enemy::get_hp_points() {
    return m_hp_points;
}
