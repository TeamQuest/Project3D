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
    moveSpeed_ = moveSpeed;
    rotationSpeed_ = rotationSpeed;
    bounds_ = bounds;
}

void Moveable::Update(float timeStep)
{
    node_->Translate(Vector3::FORWARD * moveSpeed_ * timeStep);

    // If in risk of going outside the plane, rotate the model right
    Vector3 pos = node_->GetPosition();
    if (pos.x_ < bounds_.min_.x_ || pos.x_ > bounds_.max_.x_ || pos.z_ < bounds_.min_.z_ || pos.z_ > bounds_.max_.z_)
        node_->Yaw(rotationSpeed_ * timeStep);

    // Get the model's first (only) animation state and advance its time. Note the convenience accessor to other components
    // in the same scene node
    auto* model = node_->GetComponent<AnimatedModel>(true);
    if (model->GetNumAnimationStates()) {
        AnimationState* state = model->GetAnimationStates()[0];
        state->AddTime(timeStep);
    }
}