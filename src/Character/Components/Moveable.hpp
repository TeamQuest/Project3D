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
    float get_move_speed() const
    {
        return moveSpeed_;
    }

    void stop()
    {
        moveSpeed_= rotationSpeed_ = 0;
    }
     
    void un_stop()
    {
        moveSpeed_ = moveSpeedCopy_;
    }

    bool if_focus() const 
    {
        return moveSpeed_ == 0;
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
    /// Hold orginal speed which was set during init.
    float moveSpeedCopy_;
    /// Rotation speed.
    float rotationSpeed_;
    /// Hold orginal speed which was set during init.
    float rotationSpeedCopy_;
    /// Movement boundaries.
    BoundingBox bounds_;
};