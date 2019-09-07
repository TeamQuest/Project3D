#include "Utility/InteractionCollider.hpp"

#include "Constants.hpp"
#include "Items/Inventory.hpp"
#include "Character/Npc.hpp"
#include "Items/Lootable.hpp"
#include "Items/Pickable.hpp"
#include "Quests/QuestGiver.hpp"
#include "Enemies/Enemy.hpp"
#include "Utility/Common.hpp"
#include "Scenes/Scenes.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Graphics/Camera.h>
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
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>

#pragma clang diagnostic pop

#include <algorithm>
#include <Urho3D/Graphics/AnimationController.h>

using namespace Urho3D;

InteractionCollider::InteractionCollider(Context* context) : LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void InteractionCollider::Start()
{
    {
        auto interaction_node = node_->CreateChild("Interaction");
        auto rigidbody = interaction_node->CreateComponent<RigidBody>();
        rigidbody->SetTrigger(true);
        rigidbody->SetKinematic(true);
        rigidbody->SetCollisionLayerAndMask(1, 1);
        auto collider = interaction_node->CreateComponent<CollisionShape>();
        collider->SetBox({0.5f, 2.f, 2.f}, {0.f, 1.f, 1.5f});
    }
    {
        m_window = *make<Window>(context_)
                        .name("LootWindow")
                        .styleauto()
                        .layout(LM_VERTICAL, 10, IntRect{10, 10, 10, 10})
                        .aligned(HA_LEFT, VA_CENTER)
                        .position(100, 0);
        m_window->SetEnabledRecursive(false);
        m_window->SetVisible(false);
        GetSubsystem<UI>()->GetRoot()->AddChild(m_window);
    }
    SubscribeToEvent(node_->GetChild("Interaction"), E_NODECOLLISIONEND, [this](auto, VariantMap& event_data) {
        auto node = get<Node>(event_data[NodeCollisionEnd::P_OTHERNODE]);
        if (auto spotlight_to_remove = node->GetChild("SpotlightOnSelection")) {
            if(auto npc = m_highlighted->GetComponent<Npc>()){
                 npc->resume();
            }
            close_window();
            node->RemoveChild(spotlight_to_remove);
            m_highlighted.Reset();
        }
    });
    // auto children = m_window->GetChildren();
    // for (auto child : children) {
    //     SubscribeToEvent(child, E_RELEASED, URHO3D_HANDLER(InteractionCollider, item_clicked));
    // }
}

void InteractionCollider::Update(float /* time_step */)
{
    handle_collision();
    handle_interaction();
    handle_collision_with_npc();
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
    // If there is a closer body than the highlighted one
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
    auto input = GetSubsystem<Input>();
    if (input->GetKeyPress(KEY_E)) {
        const auto world = node_->GetScene()->GetComponent<PhysicsWorld>();
        PODVector<RigidBody*> bodies;
        world->GetCollidingBodies(bodies, node_->GetChild("Interaction")->GetComponent<RigidBody>());
        for (auto&& body : bodies) {
            if (auto runner = body->GetNode()->GetComponent<QuestGiver>()) {
                close_window();
                runner->handle_window(m_window);
                return;
            }
        }
    }
    if (m_highlighted && GetSubsystem<Input>()->GetKeyPress(KEY_E)) {
        close_window();
        open_window();
    }

    if (input->GetKeyPress(KEY_K)) {
        const auto world = node_->GetScene()->GetComponent<PhysicsWorld>();
        PODVector<RigidBody*> bodies;
        world->GetCollidingBodies(bodies, node_->GetChild("Interaction")->GetComponent<RigidBody>());
        for (auto&& body : bodies) {
            if (auto enemy = body->GetNode()->GetComponent<Enemy>()) {
                URHO3D_LOGWARNING("Kick...");
                enemy->set_hp_points(enemy->get_hp_points() - 20);
                auto anim_ctrl = GetScene()->GetChild("Enemy1")->GetComponent<AnimationController>(true);
                anim_ctrl->PlayExclusive("Models/NinjaSnowWar/Ninja_Block.ani", 0, false, 0.2);
                if( enemy->get_hp_points() <= 0) {
                    anim_ctrl->PlayExclusive("Models/NinjaSnowWar/Ninja_Death1.ani", 0, false, 0.2);
                    body->GetComponent<CollisionShape>()->Remove();
                    enemy->assign_target(nullptr);
                    GetScene()->SetGlobalVar("is_joe_killed", true);
                }
                return;
            }
        }
    }

    if (input->GetKeyPress(KEY_L)) {
        const auto world = node_->GetScene()->GetComponent<PhysicsWorld>();
        PODVector<RigidBody*> bodies;
        world->GetCollidingBodies(bodies, node_->GetChild("Interaction")->GetComponent<RigidBody>());
        for (auto&& body : bodies) {
            if (auto enemy = body->GetNode()->GetComponent<Enemy>()) {
                URHO3D_LOGWARNING("Slide...");
                enemy->set_hp_points(enemy->get_hp_points() - 25);
                auto anim_ctrl = GetScene()->GetChild("Enemy1")->GetComponent<AnimationController>(true);
                anim_ctrl->PlayExclusive("Models/NinjaSnowWar/Ninja_Spin.ani", 0, false, 0.2);
                if( enemy->get_hp_points() <= 0) {
                    anim_ctrl->PlayExclusive("Models/NinjaSnowWar/Ninja_Death2.ani", 0, false, 0.2);
                    body->GetComponent<CollisionShape>()->Remove();
                    enemy->assign_target(nullptr);
                    enemy->SetGlobalVar("is_joe_killed", true);
                }
                return;
            }
        }
    }

    if (input->GetKeyPress(KEY_P)) {
        const auto world = node_->GetScene()->GetComponent<PhysicsWorld>();
        PODVector<RigidBody*> bodies;
        world->GetCollidingBodies(bodies, node_->GetChild("Interaction")->GetComponent<RigidBody>());
        for (auto&& body : bodies) {
            if (auto enemy = body->GetNode()->GetComponent<Enemy>()) {
                URHO3D_LOGWARNING("Punch...");
                enemy->set_hp_points(enemy->get_hp_points() - 10);
                auto anim_ctrl = GetScene()->GetChild("Enemy1")->GetComponent<AnimationController>(true);
                anim_ctrl->PlayExclusive("Models/NinjaSnowWar/Ninja_Spin.ani", 0, false, 0.2);
                if( enemy->get_hp_points() <= 0) {
                    anim_ctrl->PlayExclusive("Models/NinjaSnowWar/Ninja_Death2.ani", 0, false, 0.2);
                    body->GetComponent<CollisionShape>()->Remove();
                    enemy->assign_target(nullptr);
                    enemy->SetGlobalVar("is_joe_killed", true);
                }
                return;
            }
        }
    }
}

void InteractionCollider::open_window()
{
    m_window->SetEnabledRecursive(true);
    m_window->SetVisible(true);
    if (auto lootable_item = m_highlighted->GetComponent<Lootable>()) {
        const auto window_title = *make<Text>(context_).styleauto().text("<Lootable object>");
        m_window->AddChild(window_title);
        if (lootable_item->get_items().empty()) {
            // No items to display
            // TODO: Maybe show an empty window
            //       but remember to focus correctly
            return;
        }

        const auto anonymous_pro_font = GetSubsystem<ResourceCache>()->GetResource<Font>(("Fonts/Anonymous Pro.ttf"));
        for (const auto& item : lootable_item->get_items()) {
            auto item_button = *make<Button>(context_).styleauto().fixedheight(50).minwidth(item->get_name().Length() * 15);
            auto item_text = *make<Text>(context_)
                                  .text(item->get_name())
                                  .font(anonymous_pro_font)
                                  .fontsize(15)
                                  .alignment(HA_CENTER, VA_CENTER)
                                  .textaligned(HA_CENTER);
            item_button->AddChild(item_text);
            item_button->SetVar("item", item.Get());
            SubscribeToEvent(item_button, E_RELEASED, [this](auto, auto event_data) {
                auto button = static_cast<Button*>(event_data[Released::P_ELEMENT].GetPtr());
                auto item = get<Pickable>(button->GetVar("item"));
                if (handle_item_clicked(SharedPtr<Pickable>(item))) {
                    button->Remove();
                }
            });
            m_window->AddChild(item_button);
        }
        // The 0th element is the Text Label
        m_window->GetChild(1)->SetFocus(true);
    }
}

void InteractionCollider::close_window()
{
    if (m_window->IsEnabled()) {
        const auto loot_window = GetSubsystem<UI>()->GetRoot()->GetChild("LootWindow", false);
        loot_window->RemoveAllChildren();
        m_window->SetEnabledRecursive(false);
        m_window->SetVisible(false);
    }
}

bool InteractionCollider::handle_item_clicked(const SharedPtr<Pickable>& item)
{
    ////// Uncomment code below when using event signals:
    // URHO3D_LOGINFO("InteractionCollider::item_clicked");
    // auto&& event_data = GetEventDataMap();
    // event_data[ItemClickedEvent::P_ITEM] = item;
    // SendEvent(E_ITEM_CLICKED, event_data);
    //////

    if (auto inv = node_->GetComponent<Inventory>()) {
        if (inv->add(item)) {
            m_highlighted->GetComponent<Lootable>()->remove_item(item);
            return true;
        }
        else {
            URHO3D_LOGWARNING("Inventory is full");
        }
    }
    return false;
}

void InteractionCollider::handle_collision_with_npc()
{
    const auto world = node_->GetScene()->GetComponent<PhysicsWorld>();
    PODVector<RigidBody*> bodies;
    world->GetCollidingBodies(bodies, node_->GetChild("Interaction")->GetComponent<RigidBody>());
    for (auto body : bodies) {
        if (auto npc = body->GetNode()->GetComponent<Npc>()) {
            if (!npc->focused()) {
                npc->stop_walking();
            }
            if (m_highlighted) {
                m_highlighted->RemoveChild(m_highlighted->GetChild("SpotlightOnSelection"));
                m_highlighted.Reset();
            }
            m_highlighted = npc->GetNode();
            m_highlighted->CreateChild("SpotlightOnSelection");
            return;
        }
    }
}
