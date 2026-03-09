#include "Connector.h"
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/ECS/GameObject.h>
#include <string>

using ThisClass = Connector;
RTB_REGISTER_COMPONENT(Connector)
    RTB_PROPERTY_GAMEOBJECT(targetRef)
RTB_END_REGISTER(Connector)

Connector::Connector() {}
Connector::~Connector() {}

void Connector::OnAwake() {}

void Connector::OnStart()
{
    RTB_INFO("Connector::OnStart");
    if (targetRef) {
        RTB_INFO("Object connected: " + targetRef->GetName());
    }
    else {
        RTB_WARN("Connector: no target assigned.");
    }
}

void Connector::OnUpdate(float deltaTime) {}
void Connector::OnFixedUpdate(float fixedDeltaTime) {}
void Connector::OnDestroy() {}
