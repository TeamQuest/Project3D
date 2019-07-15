#include <Urho3D/Core/Context.h>

template <typename Component, typename... Args>
void register_component(Urho3D::Context* context, Args&&... args)
{
    context->RegisterFactory<Component>(args...);
}
