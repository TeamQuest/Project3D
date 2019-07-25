#include "Gameplay.hpp"

#include "Character/Npc.hpp"
#include "Items/Lootable.hpp"
#include "Items/Pickable.hpp"
#include "Utility/FPSCounter.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
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

using namespace Urho3D;

Gameplay::Gameplay(Context* context) : State(context, Scenes::Gameplay)
{
    URHO3D_LOGINFO("Gameplay scene enabled");

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
        auto exit_game_button = new Button(context_);
        exit_game_button->SetName("ExitGameButton");
        exit_game_button->SetStyle("Button");
        exit_game_button->SetSize(100, 60);
        exit_game_button->SetAlignment(HA_LEFT, VA_TOP);
        exit_game_button->SetPosition(100, 100);

        auto exit_game_label = new Text(context_);
        exit_game_label->SetName("ExitGameLabel");
        exit_game_label->SetText("exit");
        exit_game_label->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 17);
        exit_game_label->SetAlignment(HA_CENTER, VA_CENTER);
        exit_game_button->AddChild(exit_game_label);
        ui_root->AddChild(exit_game_button);
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
            std::vector<String> possible_items = {"Health Potion (+40%)", "Old trousers", "Bottle of wine", "Shoes", "Gold (1000GP)", "Gold (9GP)"};
            for (int i = 0; i < Random(1, 6); ++i) {
                const auto& random_name = possible_items[Random(0, possible_items.size())];
                auto item = new Pickable(context_);
                item->set_name(random_name);
                lootable->add_item(item);
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
            auto npc = scene->CreateChild("jack");
            npc->SetPosition({0.f, 0.f, 1.f});
            npc->CreateComponent<Npc>();
        }
    }
}

void Gameplay::handle_key_down(Urho3D::StringHash /* event_type */, Urho3D::VariantMap& event_data)
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
    if (m_character) {
        m_character->handle_movement();
        m_character->adjust_head_pitch();

        if (GetSubsystem<Input>()->IsMouseVisible() || GetSubsystem<UI>()->GetFocusElement()) {
            return;
        }

        if (auto camera = m_camera.Lock()) {
            m_character->handle_camera(camera, scene->GetComponent<PhysicsWorld>());
        }
    }
}

Gameplay::~Gameplay()
{
    URHO3D_LOGINFO("Gameplay scene disabled");
}
