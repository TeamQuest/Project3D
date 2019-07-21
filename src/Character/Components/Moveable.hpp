#pragma once

#include <Urho3D/Math/BoundingBox.h>
#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

class Moveable : public LogicComponent {
    URHO3D_OBJECT(Moveable, LogicComponent);

public:
    /// Construct.
    explicit Moveable(Context* context);

    /// Set motion parameters: forward movement speed, rotation speed, and movement boundaries.
    void SetParameters(float moveSpeed, float rotationSpeed, const BoundingBox& bounds);
    /// Handle scene update. Called by LogicComponent base class.
    void Update(float timeStep) override;

    /// Return forward movement speed.
    float GetMoveSpeed() const
    {
        return moveSpeed_;
    }
    /// Return rotation speed.
    float GetRotationSpeed() const
    {
        return rotationSpeed_;
    }
    /// Return movement boundaries.
    const BoundingBox& GetBounds() const
    {
        return bounds_;
    }

private:
    /// Forward movement speed.
    float moveSpeed_;
    /// Rotation speed.
    float rotationSpeed_;
    /// Movement boundaries.
    BoundingBox bounds_;
};