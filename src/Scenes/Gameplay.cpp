#include "Scenes/Gameplay.hpp"

#include "Character/Status.hpp"
#include "Constants.hpp"
#include "HUD/Hud.hpp"
#include "Items/Gold.hpp"
#include "Items/HpPotion.hpp"
#include "Character/Npc.hpp"
#include "Items/Lootable.hpp"
#include "Items/Sword.hpp"
#include "Scenes/Scenes.hpp"
#include "Utility/Common.hpp"
#include "Utility/FPSCounter.hpp"
#include "Quests/QuestGiver.hpp"
#include "Quests/QuestRunner.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/Animation.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>

#pragma clang diagnostic pop

#include <cmath>
#include <vector>
#include <Enemies/Enemy.hpp>

using namespace Urho3D;

Gameplay::Gameplay(Context* context) : State(context, Scenes::Gameplay)
{
    URHO3D_LOGINFO("Gameplay scene enabled");

    GetSubsystem<Input>()->SetMouseVisible(true);

    init_ui();
    init_gamescene();
}

void Gameplay::init_ui()
{
    {  // setup_scene_components
        scene->CreateComponent<FPSCounter>();
        scene->CreateComponent<Hud>();
    }
    const auto cache = GetSubsystem<ResourceCache>();
    auto ui_root = GetSubsystem<UI>()->GetRoot();
    ui_root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    {  // ExitGame button and label
        auto exit_button = *make<Button>(context_)
                                .name("ExitGameButton")
                                .style("Button")
                                .size(100, 60)
                                .alignment(HorizontalAlignment::HA_LEFT, VerticalAlignment::VA_TOP)
                                .position(100, 100);
        auto exit_label = *make<Text>(context_)
                               .name("ExitGameLabel")
                               .text("exit")
                               .font(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 17)
                               .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER);
        exit_button->AddChild(exit_label);
        ui_root->AddChild(exit_button);
    }

    SubscribeToEvent(ui_root->GetChild("ExitGameButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_MENUREQUESTED); });
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Gameplay, handle_key_down));
    SubscribeToEvent(E_POSTRENDERUPDATE, [&](auto&&...) { scene->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true); });
}

void Gameplay::init_gamescene()
{
    const auto cache = GetSubsystem<ResourceCache>();

    {  // setup_scene_components
        scene->CreateComponent<DebugRenderer>();
        scene->CreateComponent<Octree>();
        scene->CreateComponent<PhysicsWorld>();
    }

    { /* Character */
        auto jack = scene->CreateChild(PLAYER_NAME);
        jack->SetPosition({0.f, -2.5f, -8.f});
        m_character = jack->CreateComponent<Character>();
    }

    { /* Camera */
        m_camera = scene->CreateChild("Camera");
        auto camera = m_camera->CreateComponent<Camera>();
        camera->SetFarClip(2000);
        auto light = m_camera->CreateComponent<Light>();
        light->SetLightType(LIGHT_POINT);
        light->SetRange(10.f);
    }

    {  // setup_viewport
        auto renderer = GetSubsystem<Renderer>();
        auto viewport = MakeShared<Viewport>(context_, scene.Get(), m_camera->GetComponent<Camera>());
        renderer->SetViewport(0, viewport);
    }

    { /* Ground */
        auto floor = scene->CreateChild("Floor");
        floor->SetPosition(Vector3(0.f, -5.f, 0.f));
        floor->SetScale(Vector3(500.f, 1.f, 500.f));
        auto floor_model = floor->CreateComponent<StaticModel>();
        floor_model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        floor_model->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));
        auto body = floor->CreateComponent<RigidBody>();
        // TODO: Explain this and replace raw number with an enum.
        //       Collision Layer is set to allow collision with the camera raycast.
        // This means that the camera behind the player never goes under ground.
        body->SetCollisionLayer(2);
        body->SetMass(0.f);
        auto collider = floor->CreateComponent<CollisionShape>();
        collider->SetBox(Vector3::ONE);
    }

    { /* Sky */
        auto sky = scene->CreateChild("Sky");
        auto skybox = sky->CreateComponent<Skybox>();
        skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));
    }

    { /* Boxes */
        constexpr auto NUM_OBJECTS = 1000u;
        for (unsigned i = 0; i < NUM_OBJECTS; ++i) {
            auto box = scene->CreateChild("Box");
            box->SetPosition(Vector3(Random(200.f) - 100.f, Random(200.f) + 5.f, Random(200.f) - 100.f));
            box->SetRotation(Quaternion(Random(360.f), Random(360.f), Random(360.f)));
            box->SetScale(0.5f);

            auto rigidbody = box->CreateComponent<RigidBody>();
            rigidbody->SetMass(1.f);

            auto collider = box->CreateComponent<CollisionShape>();
            collider->SetBox(Vector3::ONE);

            auto lootable = box->CreateComponent<Lootable>();
            for (int j = 0; j < Random(1, 3); ++j) {
                auto gold_coins = MakeShared<Gold>(context_);
                auto random_amount = Random(100, 1000);
                gold_coins->set_name(ToString("%d gold coins", random_amount));
                gold_coins->set_description("Gold coins");
                gold_coins->set_amount(random_amount);
                lootable->add_item(gold_coins);
            }

            for (int k = 0; k < Random(0, 2); ++k) {
                auto sword = MakeShared<Sword>(context_);
                auto random_dmg = Random(3, 12);
                sword->set_name("Medieval sword");
                sword->set_description("Beautiful sword");
                sword->set_dmg(random_dmg);
                lootable->add_item(sword);
            }

            for (int k = 0; k < Random(0, 3); ++k) {
                auto hp_potion = MakeShared<HpPotion>(context_);
                hp_potion->set_name("Health Point Potion");
                hp_potion->set_description("Hp Potion");
                lootable->add_item(hp_potion);
            }

            auto box_model = box->CreateComponent<StaticModel>();
            box_model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
            box_model->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
        }
        scene->GetChild("Box", false)->SetPosition(Vector3::FORWARD);
    }

    { /* NPC's */
        constexpr auto NUM_NPC = 1u;
        for (unsigned i = 0; i < NUM_NPC; ++i) {
            auto npc = scene->CreateChild("Jill" + String(i));
            npc->SetPosition({0.f, 0.f, 0.f});
            npc->CreateComponent<Npc>();
        }
    }
    { /* Ninja */
        auto ninja = scene->CreateChild("Ninja1");
        ninja->LoadXML(cache->GetResource<XMLFile>("Objects/Ninja1.xml")->GetRoot());
        auto anim_ctrl = ninja->GetComponent<AnimationController>(true);
        anim_ctrl->PlayExclusive("Models/NinjaSnowWar/Ninja_Idle3.ani", 0, true, 0.2);
        ninja->SetPosition(Vector3(0.f, -1.f, 4.f));
        ninja->SetRotation(Quaternion(180.f, Vector3::UP));
        auto quest_giver = ninja->CreateComponent<QuestGiver>();
        auto _1st_quest = new FirstQuest{context_};
        auto _2nd_quest = new SecondQuest{context_};
        quest_giver->assign_quest(_1st_quest);
        quest_giver->assign_quest(_2nd_quest);
        ninja->SetName("Ninja1");
    }

    { /* Enemy */
        auto ninja2 = scene->CreateChild("Enemy1");
        ninja2->CreateComponent<Enemy>();
//        ninja2->LoadXML(cache->GetResource<XMLFile>("Objects/Enemy.xml")->GetRoot());
        auto anim_ctrl = ninja2->CreateComponent<AnimationController>();
        anim_ctrl->PlayExclusive("Models/NinjaSnowWar/Ninja_Attack2.ani", 0, true, 0.2);
        ninja2->SetPosition({6.f, 0.f, 0.f});
        ninja2->SetRotation(Quaternion(140.f, Vector3::UP));
        ninja2->SetName("Enemy1");
    }
    auto place_wall = [&](const String& name, const Vector3& position, const Quaternion& rotation, const Vector3& scale) {
        /* Walls */
        auto wall = scene->CreateChild(name);
        wall->SetPosition(position);
        wall->SetRotation(rotation);
        wall->SetScale(scale);

        auto rigidbody = wall->CreateComponent<RigidBody>();
        rigidbody->SetMass(0.f);
        rigidbody->SetCollisionLayer(2);

        auto collider = wall->CreateComponent<CollisionShape>();
        collider->SetBox(Vector3::ONE);

        auto box_model = wall->CreateComponent<StaticModel>();
        box_model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        box_model->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
        return wall;
    };
    place_wall("Ceiling",
               Vector3(0.f, 10.f, 0.f),
               Quaternion(0.f, 0.f, 0.f),
               Vector3(200.f, 10.5f, 200.f)
    );
    place_wall("Wall_1",
               Vector3(-3.f, -2.5f, 0.f),
               Quaternion(90.f, 90.f, 0.f),
               Vector3(20.f, 0.5f, 6.f)
    );
    place_wall("Wall_2",
               Vector3(3.f, -2.5f, -5.f),
               Quaternion(90.f, 90.f, 0.f),
               Vector3(15.f, 0.5f, 6.f)
    );
    place_wall("Wall_3",
               Vector3(9.5f, -2.5f, 10.f),
               Quaternion(90.f, 0.f, 0.f),
               Vector3(25.f, 0.5f, 6.f)
    );
    place_wall("Wall_4",
               Vector3(3.f, -2.5f, 10.f),
               Quaternion(90.f, 90.f, 0.f),
               Vector3(8.f, 0.5f, 6.f)
    );
    place_wall("Wall_5",
               Vector3(-3.f, -2.5f, -10.f),
               Quaternion(90.f, 0.f, 0.f),
               Vector3(12.f, 0.5f, 6.f)
    );
    place_wall("Wall_6",
               Vector3(15.3f, -2.5f, 6.25f),
               Quaternion(90.f, 0.f, 0.f),
               Vector3(25.f, 0.5f, 6.f)
    );
    place_wall("Wall_7",
               Vector3(14.f, -2.5f, 2.25f),
               Quaternion(90.f, 0.f, 0.f),
               Vector3(22.f, 0.5f, 6.f)
    );
    place_wall("Wall_8",
               Vector3(27.f, -2.5f, 6.25f),
               Quaternion(90.f, 90.f, 0.f),
               Vector3(50.f, 0.5f, 6.f)
    );
    place_wall("Wall_9",
               Vector3(17.f, -2.5f, -8.25f),
               Quaternion(90.f, 0.f, 0.f),
               Vector3(22.f, 0.5f, 6.f)
    );
    place_wall("Wall_10",
               Vector3(8.5f, -2.5f, -12.3f),
               Quaternion(90.f, 0.f, 0.f),
               Vector3(12.f, 0.5f, 6.f)
    );
    place_wall("Wall_11",
               Vector3(19.f, -2.5f, -12.3f),
               Quaternion(90.f, 0.f, 0.f),
               Vector3(9.f, 0.5f, 6.f)
    );
    place_wall("Wall_12",
               Vector3(16.f, -2.5f, -18.25f),
               Quaternion(90.f, 0.f, 0.f),
               Vector3(26.f, 0.5f, 6.f)
    );
    place_wall("Wall_13",
               Vector3(3.f, -2.5f, -20.f),
               Quaternion(90.f, 90.f, 0.f),
               Vector3(15.f, 0.5f, 6.f)
    );
    {  /* Status component */
        if (auto status_component = scene->GetChild(PLAYER_NAME)->GetComponent<Status>()) {
            auto character_name_label = *make<Text>(context_)
                    .name("CharacterName")
                    .font(GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 50)
                    .text(status_component->get_character_name())
                    .texteffect(TextEffect::TE_STROKE)
                    .effectstrokethickness(3)
                    .effectcolor(Color(0.f, 0.f, 0.f))
                    .color(Color(1.f, 1.f, 1.f))
                    .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER)
                    .position(0, -100);
            GetSubsystem<UI>()->GetRoot()->AddChild(character_name_label);
        }
    }
}

void Gameplay::handle_key_down(StringHash /* event_type */, VariantMap& event_data)
{
    int key = event_data[KeyDown::P_KEY].GetInt();
    switch (key) {
        case KEY_TAB: {
            const auto is_mouse_visible = GetSubsystem<Input>()->IsMouseVisible();
            GetSubsystem<Input>()->SetMouseVisible(!is_mouse_visible);
            break;
        }
        case KEY_R: {
            auto character = scene->GetChild(PLAYER_NAME)->GetComponent<Character>();
            for (auto [quest_name, quest] : character->GetComponent<QuestRunner>()->get_quests()) {
                URHO3D_LOGWARNINGF("Quest: %s, address: %p", quest_name.CString(), quest);
            }
        }
        /* TO DELETE , ONLY DEBUG */
        case KEY_M: {
            auto npc = scene->GetChild("Jill0")->GetComponent<Npc>();
            npc->follow(scene->GetChild(PLAYER_NAME));
        }
    }
}

void Gameplay::update(float /* time_step */)
{
    ///// Does it need to be here?
    // if (GetSubsystem<UI>()->GetFocusElement()) {
    //     return;
    // }
    {  /* DEBUG WALLS */
//        static auto wall_txt = [&]() -> Text * {
//            auto txt = *make<Text>(context_)
//                    .text("wall name: None")
//                    .name("wallid")
//                    .font(GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/gta5.ttf"), 50)
//                    .texteffect(TextEffect::TE_STROKE)
//                    .effectstrokethickness(5)
//                    .effectcolor(Color(0.f, 0.f, 0.f))
//                    .color(Color(1.f, 1.f, 1.f))
//                    .alignment(HA_RIGHT, VA_TOP)
//                    .position(-40, 20);
//            scene->GetSubsystem<UI>()->GetRoot()->AddChild(txt);
//            return txt;
//        }();
//        const auto ray_dir = m_camera->GetDirection();
//        auto ray_distance = 1000.f;
//        PhysicsRaycastResult raycast;
//        auto ray = Ray(m_character->GetNode()->GetPosition() + Vector3(0, 2, 0), ray_dir);
//        scene->GetComponent<PhysicsWorld>()->RaycastSingle(raycast, ray, ray_distance, 2);
//        if (raycast.body_ && raycast.body_->GetNode()->GetName() != "Floor") {
//            wall_txt->SetText("wall name: " + raycast.body_->GetNode()->GetName() + "\n" + String(raycast.distance_));
//        } else {
//            wall_txt->SetText("wall name: None");
//        }
    }
    if (m_character) {
        m_character->handle_movement();
        m_character->adjust_head_pitch();

        if (auto camera = m_camera.Lock()) {
            m_character->handle_camera(camera, scene->GetComponent<PhysicsWorld>());
        }
    }
}

Gameplay::~Gameplay()
{
    URHO3D_LOGINFO("Gameplay scene disabled");
}
