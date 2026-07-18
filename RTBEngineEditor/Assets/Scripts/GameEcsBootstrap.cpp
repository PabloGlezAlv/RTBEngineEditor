#include "ProjectileSimulation.h"

#include <RTBEngine/ECS/World.h>
#include <RTBEngine/Scripting/ScriptBridgeABI.h>

extern "C" __declspec(dllexport)
void RTBScripts_InitializeEcs(RTBEngine::ECS::World* world)
{
    if (!world) {
        return;
    }

    RTBEngine::ECS::RegisterProjectileSystems(*world);
}
