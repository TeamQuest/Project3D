#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_STARTGAME, StartGameEvent)
{
    URHO3D_PARAM(P_NODE, Node);
    // URHO3D_PARAM(P_SENDER, Sender);
    // URHO3D_PARAM(P_MESSAGE, Message);
}

URHO3D_EVENT(E_MENUREQUESTED, RequestMenuEvent)
{
    URHO3D_PARAM(P_STATE, State);
}

URHO3D_EVENT(E_OPTIONS, RequestOptionsEvent)
{
    URHO3D_PARAM(P_STATE, State);
}

enum class Scenes { Empty, MainMenu, Options, Gameplay };
