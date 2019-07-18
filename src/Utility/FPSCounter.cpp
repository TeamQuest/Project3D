#include "Utility/FPSCounter.hpp"

#include "Utility/Common.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/StringUtils.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#pragma clang diagnostic pop

using namespace Urho3D;

FPSCounter::FPSCounter(Context* context) : LogicComponent(context)
{
    auto fps_text = *make<Text>(context)
                         .name("FPS")
                         .font(GetSubsystem<ResourceCache>()->GetResource<Font>("Fonts/gta5.ttf"), 50)
                         .text("FPS: 0")
                         .texteffect(TextEffect::TE_STROKE)
                         .effectstrokethickness(5)
                         .effectcolor(Color(0.f, 0.f, 0.f))
                         .color(Color(1.f, 1.f, 1.f))
                         .alignment(HA_LEFT, VA_TOP)
                         .position(40, 20);
    GetSubsystem<UI>()->GetRoot()->AddChild(fps_text);
}

FPSCounter::~FPSCounter()
{
}

void FPSCounter::Update(float time_step)
{
    if ((m_counter += time_step) > FPS_UPDATE_TIME) {
        auto fps_text = static_cast<Text*>(GetSubsystem<UI>()->GetRoot()->GetChild("FPS", false));
        fps_text->SetText(ToString("FPS: %f", std::roundf(m_counter / time_step / FPS_UPDATE_TIME)));
        m_counter = 0;
    }
}
