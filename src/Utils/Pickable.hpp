#pragma once

#include <Urho3D/Scene/LogicComponent.h>

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;

/// Custom logic component for rotating a scene node.
class Pickable : public LogicComponent {
    URHO3D_OBJECT(Pickable, LogicComponent);

public:
    /// Construct.
    Pickable(Context* context);

    void Update(float timeStep) override;

    void SetMessage(String msg);

    String GetMessage() const;

private:
    String m_msg;
};
