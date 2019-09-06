#include "Scenes/Gameplay.hpp"

#include "Items/Gold.hpp"
#include "Items/Lootable.hpp"
#include "Utility/Common.hpp"
#include "Utility/FPSCounter.hpp"
#include "HUD/Hud.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
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
#include <Urho3D/Scene/Scene.h>

#pragma clang diagnostic pop

#include <cmath>
#include <vector>
#include <Items/Sword.hpp>
#include <Items/HpPotion.hpp>
#include <Character/Status.hpp>
#include <Scenes/Scenes.hpp>

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
    {  // setup_hud
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
        auto jack = scene->CreateChild("jack");
        jack->SetPosition({0.f, 0.f, 1.f});
        m_character = jack->CreateComponent<Character>();

        auto status_comp = scene->GetComponent<Status>();
        m_character->GetNode()->CloneComponent(status_comp);
//        status_comp->Remove();
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
    {
        if (auto status_component = scene->GetChild("jack")->GetComponent<Status>()) {
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
