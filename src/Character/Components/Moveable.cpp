#include "Character/Components/Moveable.hpp"

#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationState.h>
#include <Urho3D/Scene/Scene.h>

#include <algorithm>

using namespace Urho3D;

Moveable::Moveable(Context* context) : LogicComponent(context), moveSpeed_(0.0f), rotationSpeed_(0.0f)
{
}

void Moveable::SetParameters(float moveSpeed, float rotationSpeed, const BoundingBox& bounds)
{
    moveSpeed_ = moveSpeedCopy_ = moveSpeed;
    rotationSpeed_ = rotationSpeedCopy_ = rotationSpeed;
    bounds_ = bounds;
}

void Moveable::Update(float timeStep)
{
}