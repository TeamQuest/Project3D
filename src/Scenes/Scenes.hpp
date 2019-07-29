#pragma once

#include <Urho3D/Core/Object.h>

URHO3D_EVENT(E_STARTGAME, StartGameEvent)
{
    URHO3D_PARAM(P_NODE, Node);
}

URHO3D_EVENT(E_MENUREQUESTED, RequestMenuEvent)
{
}

URHO3D_EVENT(E_OPTIONSREQUESTED, RequestOptionsEvent)
{
}

URHO3D_EVENT(E_ITEM_CLICKED, ItemClickedEvent)
{
    URHO3D_PARAM(P_ITEM, Item);
}

enum class Scenes { Empty, MainMenu, Options, Gameplay };
