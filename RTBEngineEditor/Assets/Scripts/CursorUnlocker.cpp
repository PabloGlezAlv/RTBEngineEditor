#include "CursorUnlocker.h"

#include <RTBEngine/Input/InputManager.h>

using ThisClass = CursorUnlocker;

RTB_REGISTER_COMPONENT(CursorUnlocker)
RTB_END_REGISTER(CursorUnlocker)

void CursorUnlocker::OnStart()
{
    Apply();
}

void CursorUnlocker::OnValidate()
{
    Apply();
}

void CursorUnlocker::Apply() const
{
    RTBEngine::Input::InputManager::GetInstance().SetMouseRelativeMode(false);
}
