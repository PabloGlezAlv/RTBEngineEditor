#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class EnemySpawnPoint : public RTBEngine::ECS::Component
{
public:
    EnemySpawnPoint() = default;
    ~EnemySpawnPoint() override = default;

    RTB_COMPONENT(EnemySpawnPoint)
};
