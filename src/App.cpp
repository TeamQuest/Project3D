#include "App.hpp"

#include "Utility/Pickable.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>

#pragma clang diagnostic pop

using namespace Urho3D;

App::App(Context* context) : Application{context}, m_scene{new Scene{context}}
{
    context->RegisterFactory<Character>();
    context->RegisterFactory<Pickable>();
}

void App::Setup()
{
    engineParameters_["FullScreen"] = false;
    engineParameters_["WindowWidth"] = 1280;
    engineParameters_["WindowHeight"] = 720;
    engineParameters_["WindowResizable"] = false;
    engineParameters_["VSync"] = true;
    engine_->SetMaxInactiveFps(20);
}

void App::Start()
{
    GetSubsystem<Input>()->SetMouseVisible(true);

    m_scene->CreateComponent<DebugRenderer>();
    m_scene->CreateComponent<Octree>();
    m_scene->CreateComponent<PhysicsWorld>();

    init_user_interface();
    init_scene();

    {
        auto renderer = GetSubsystem<Renderer>();
        auto viewport = MakeShared<Viewport>(context_, m_scene, m_camera->GetComponent<Camera>());
        renderer->SetViewport(0, viewport);
    }
    create_character();

    SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(App, handle_begin_frame));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(App, handle_key_down));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(App, handle_key_up));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(App, handle_update));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(App, handle_postrender_update));
    // SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(App, handle_post_update));
    // SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(App, handle_render_update));
    // SubscribeToEvent(E_ENDFRAME, URHO3D_HANDLER(App, handle_end_frame));
}

void App::Stop()
{
}

void App::create_character()
{
    auto cache = GetSubsystem<ResourceCache>();

    auto jack = m_scene->CreateChild("Jack");
    jack->SetPosition(Vector3(0.0f, 1.0f, 0.0f));

    auto asset_node = jack->CreateChild("Character Asset Node");
    asset_node->SetRotation(Quaternion(180, Vector3(0, 1, 0)));

    // Create the rendering component + animation controller
    auto model = asset_node->CreateComponent<AnimatedModel>();
    model->SetModel(cache->GetResource<Model>("Models/Mutant/Mutant.mdl"));
    model->SetMaterial(cache->GetResource<Material>("Models/Mutant/Materials/mutant_M.xml"));
    model->SetCastShadows(true);
    asset_node->CreateComponent<AnimationController>();

    // Set the head bone for manual control
    model->GetSkeleton().GetBone("Mutant:Head")->animated_ = false;

    // Create rigidbody, and set non-zero mass so that the body becomes dynamic
    auto body = jack->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);
    body->SetMass(1.0f);

    // Set zero angular factor so that physics doesn't turn the character on its own.
    // Instead we will control the character yaw manually
    body->SetAngularFactor(Vector3::ZERO);

    // Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
    body->SetCollisionEventMode(CollisionEventMode::COLLISION_ALWAYS);

    // Set a capsule shape for collision
    auto shape = jack->CreateComponent<CollisionShape>();
    shape->SetCapsule(0.7f, 1.8f, Vector3(0.0f, 0.9f, 0.0f));

    // Create the character logic component, which takes care of steering the rigidbody
    // Remember it so that we can set the controls. Use a WeakPtr because the scene hierarchy already owns it
    // and keeps it alive as long as it's not removed from the hierarchy
    m_character = jack->CreateComponent<Character>();
}

void App::handle_begin_frame(StringHash /* event_type */, VariantMap& /* event_data */)
{
}

void App::handle_key_down(StringHash /* event_type */, VariantMap& event_data)
{
    int key = event_data[KeyDown::P_KEY].GetInt();
    switch (key) {
        case KEY_ESCAPE: {
            engine_->Exit();
            break;
        }
        case KEY_TAB: {
            const auto is_mouse_visible = GetSubsystem<Input>()->IsMouseVisible();
            GetSubsystem<Input>()->SetMouseVisible(!is_mouse_visible);
            auto hint_text = dynamic_cast<Text*>(GetSubsystem<UI>()->GetRoot()->GetChild(String("Hint Text")));
            hint_text->SetText(is_mouse_visible ? "Press WSAD to move around" : "Press TAB to show/hide mouse");
            break;
        }
    }
}

void App::handle_key_up(Urho3D::StringHash /* event_type */, Urho3D::VariantMap& /* event_data */)
{
    // int key = event_data[KeyDown::P_KEY].GetInt();
    // switch (key) {

    // }
}

void App::handle_update(StringHash event_type, VariantMap& event_data)
{
    const auto time_step = event_data[Urho3D::Update::P_TIMESTEP].GetFloat();
    static auto counter = 0.f;
    constexpr auto fps_update_time = 0.5f;  // in seconds
    if ((counter += time_step) > fps_update_time) {
        auto hint_text = dynamic_cast<Text*>(GetSubsystem<UI>()->GetRoot()->GetChild(String("FPS")));
        hint_text->SetText(ToString("FPS: %f", std::roundf(counter / time_step / fps_update_time)));
        counter = 0;
    }

    if (m_character) {
        m_character->handle_movement();
    }
    handle_post_update(event_type, event_data);
}

void App::handle_post_update(StringHash /* event_type */, VariantMap& /* event_data */)
{
    if (!m_character) {
        return;
    }

    m_character->adjust_head_pitch();

    if (GetSubsystem<Input>()->IsMouseVisible() || GetSubsystem<UI>()->GetFocusElement()) {
        return;
    }
    m_character->handle_camera(m_camera, m_scene->GetComponent<PhysicsWorld>());
}

void App::handle_postrender_update(StringHash /* event_type */, VariantMap& /* event_data */)
{
    // Comment to hide Debug wireframes
    m_scene->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
}

void App::handle_closed_pressed(StringHash /* event_type */, VariantMap& /* event_data */)
{
    engine_->Exit();
}

void App::init_user_interface()
{
    auto cache = GetSubsystem<ResourceCache>();
    auto ui = GetSubsystem<UI>();
    ui->GetRoot()->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    auto quit_button = new Button(context_);
    quit_button->SetName("Quit Button");
    quit_button->SetStyle("Button");
    quit_button->SetSize(64, 64);
    quit_button->SetPosition(16, 116);
    ui->GetRoot()->AddChild(quit_button);
    SubscribeToEvent(quit_button, E_RELEASED, URHO3D_HANDLER(App, handle_closed_pressed));

    auto hint_text = new Text(context_);
    hint_text->SetName("Hint Text");
    hint_text->SetText("Press TAB to show/hide mouse");
    hint_text->SetFont(cache->GetResource<Font>("Fonts/gta5.ttf"), 50);
    hint_text->SetTextEffect(TextEffect::TE_STROKE);
    hint_text->SetEffectStrokeThickness(5);
    hint_text->SetEffectColor(Color(0.f, 0.f, 0.f));
    hint_text->SetColor(Color(1.f, 1.f, 1.f));
    hint_text->SetHorizontalAlignment(HA_CENTER);
    hint_text->SetVerticalAlignment(VA_TOP);
    ui->GetRoot()->AddChild(hint_text);

    auto text_fps = new Text(context_);
    text_fps->SetName("FPS");
    text_fps->SetFont(cache->GetResource<Font>("Fonts/gta5.ttf"), 50);
    text_fps->SetTextEffect(TextEffect::TE_STROKE);
    text_fps->SetEffectStrokeThickness(5);
    text_fps->SetEffectColor(Color(0.f, 0.f, 0.f));
    text_fps->SetColor(Color(1.f, 1.f, 1.f));
    text_fps->SetHorizontalAlignment(HorizontalAlignment::HA_LEFT);
    text_fps->SetVerticalAlignment(VerticalAlignment::VA_TOP);
    text_fps->SetPosition(40, 20);
    ui->GetRoot()->AddChild(text_fps);
}

void App::init_scene()
{
    auto cache = GetSubsystem<ResourceCache>();

    { /* Ground */
        auto floor = m_scene->CreateChild("Floor");
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

    constexpr auto NUM_OBJECTS = 100u;
    for (unsigned i = 0; i < NUM_OBJECTS; ++i) {
        auto box = m_scene->CreateChild("Box");
        box->SetPosition(Vector3(Random(200.f) - 100.f, Random(200.f) + 5.f, Random(200.f) - 100.f));
        box->SetRotation(Quaternion(Random(360.f), Random(360.f), Random(360.f)));
        box->SetScale(3.f);

        auto rigidbody = box->CreateComponent<RigidBody>();
        rigidbody->SetMass(1.f);

        auto collider = box->CreateComponent<CollisionShape>();
        collider->SetBox(Vector3::ONE);

        auto pickable = box->CreateComponent<Pickable>();
        auto possible_items = std::vector<String>{"Health Potion (+40%)", "Old trousers", "Bottle of wine", "Shoes", "Gold (1000GP)", "Gold (9GP)"};
        const auto& random_item = possible_items[Random(0, possible_items.size())];
        pickable->set_item(random_item);

        auto box_model = box->CreateComponent<StaticModel>();
        box_model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        box_model->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
    }
    {
        auto sky = m_scene->CreateChild("Sky");
        auto skybox = sky->CreateComponent<Skybox>();
        skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));
    }
    {
        m_camera = m_scene->CreateChild("Camera");
        auto camera = m_camera->CreateComponent<Camera>();
        camera->SetFarClip(2000);
        auto light = m_camera->CreateComponent<Light>();
        light->SetLightType(LIGHT_POINT);
        light->SetRange(200.f);
    }
}

URHO3D_DEFINE_APPLICATION_MAIN(App)
