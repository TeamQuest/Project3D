#include "HUD/Hud.hpp"

#include "Utility/Common.hpp"
#include "Character/Status.hpp"

#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/ProgressBar.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Scenes/Scenes.hpp>

using namespace Urho3D;

Hud::Hud(Urho3D::Context *context) : LogicComponent(context) {

    auto ui_root = GetSubsystem<UI>()->GetRoot();
    {
        auto health_bar = *make<ProgressBar>(context)
                .range(100)
                .size(500, 100)
                .name("health_bar")
                .alignment(HA_RIGHT, VA_TOP)
                .value(90)
                .color(Color::BLACK)
                .position(30, 20);
        ui_root->AddChild(health_bar);

        auto health_label = *make<Text>(context)
                .name("health_label")
                .font(GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/gta5.ttf"), 50)
                .text("HEALTH")
                .texteffect(TextEffect::TE_STROKE)
                .effectstrokethickness(5)
                .effectcolor(Color(0.f, 0.f, 0.f))
                .color(Color(1.f, 1.f, 1.f))
                .alignment(HA_RIGHT, VA_TOP)
                .position(-500, 40);
        ui_root->AddChild(health_label);
    }


    {
        auto open_inventory_button = *make<Button>(context)
                .name("OpenInventoryButton")
                .style("Button")
                .size(200, 100)
                .alignment(HA_RIGHT, VA_TOP)
                .position(-100, 220);

        auto open_inventory_label = *make<Text>(context)
                .name("OpenInventoryButtonLabel")
                .text("open inventory")
                .font(GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15)
                .alignment(HorizontalAlignment::HA_CENTER, VerticalAlignment::VA_CENTER)
                .textalignment(HorizontalAlignment::HA_CENTER);
        open_inventory_button->AddChild(open_inventory_label);
        ui_root->AddChild(open_inventory_button);

        SubscribeToEvent(ui_root->GetChild("OpenInventoryButton", false), E_RELEASED, [&](auto&&...) { SendEvent(E_OPEN_INVENTORY); });
    }
}

void Hud::Start() {
}

void Hud::Update(float time_step) {

    // update health bar
    if (auto status_component = GetScene()->GetChild("jack")->GetComponent<Status>()) {
        auto health_bar = GetSubsystem<UI>()->GetRoot()->GetChildStaticCast<ProgressBar>("health_bar", false);
        health_bar->SetValue(status_component->get_hp_points());
    }
}
