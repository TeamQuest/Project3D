#include "Scenes/Gameplay.hpp"

#include "Items/Gold.hpp"
#include "Items/Lootable.hpp"
#include "Quests/QuestGiver.hpp"
#include "Quests/QuestRunner.hpp"
#include "Utility/Common.hpp"
#include "Utility/FPSCounter.hpp"
#include "Constants.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/AnimationController.h>
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
        jack->SetPosition({0.f, 0.f, 1.f});
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
            for (int j = 0; j < Random(1, 6); ++j) {
                auto gold_coins = MakeShared<Gold>(context_);
                auto random_amount = Random(100, 1000);
                gold_coins->set_name(ToString("%d gold coins", random_amount));
                gold_coins->set_description("Gold coins");
                gold_coins->set_amount(random_amount);
                lootable->add_item(gold_coins);
            }

            auto box_model = box->CreateComponent<StaticModel>();
            box_model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
            box_model->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
        }
        scene->GetChild("Box", false)->SetPosition(Vector3::FORWARD);
    }
    { /* Ninja */
        auto ninja = scene->CreateChild("Ninja1");
        ninja->LoadXML(cache->GetResource<XMLFile>("Objects/Ninja1.xml")->GetRoot());
        auto anim_ctrl = ninja->GetComponent<AnimationController>(true);
        anim_ctrl->PlayExclusive("Models/NinjaSnowWar/Ninja_Idle3.ani", 0, true, 0.2);
        ninja->SetPosition(Vector3::FORWARD * 5.f);
        ninja->SetRotation(Quaternion(180.f, Vector3::UP));
        auto quest_giver = ninja->CreateComponent<QuestGiver>();
        auto _1st_quest = new FirstQuest{context_};
        auto _2nd_quest = new SecondQuest{context_};
        quest_giver->assign_quest(_1st_quest);
        quest_giver->assign_quest(_2nd_quest);
        ninja->SetName("Ninja1");
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
    }
}

void Gameplay::update(float /* time_step */)
{
    ///// Does it need to be here?
    // if (GetSubsystem<UI>()->GetFocusElement()) {
    //     return;
    // }
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
