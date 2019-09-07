#include "Gameover.hpp"

#include "Utility/Common.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Input/InputEvents.h>

using namespace Urho3D;

Gameover::Gameover(Urho3D::Context *context) : State(context, Scenes::GameOver)
{
    auto text = *make<Text>(context)
            .name("gameover_txt")
            .text("GAME OVER\n         3")
            .font(GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/gta5.ttf"), 200)
            .texteffect(TextEffect::TE_STROKE)
            .effectstrokethickness(5)
            .effectcolor(Color(0.f, 0.f, 0.f))
            .color(Color(1.f, 1.f, 1.f))
            .alignment(HA_CENTER, VA_CENTER);
    GetSubsystem<UI>()->GetRoot()->AddChild(text);
}

void Gameover::update(float time_step)
{
    static auto acc = 0.f;
    acc += time_step;
    auto text = GetSubsystem<UI>()->GetRoot()->GetChildStaticCast<Text>("gameover_txt", false);
    if (acc > 3.f) {
        SendEvent(E_EXITREQUESTED);
    }
    else if (acc > 2.f) {
        text->SetText("GAME OVER\n         1");
    }
    else if (acc > 1.f) {
        text->SetText("GAME OVER\n         2");
    }
}
