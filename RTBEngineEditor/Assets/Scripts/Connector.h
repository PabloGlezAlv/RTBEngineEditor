#pragma once
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class Connector : public RTBEngine::ECS::Component {
public:
    Connector();
    ~Connector() override;

    Connector(const Connector&) = delete;
    Connector& operator=(const Connector&) = delete;

    //Loop methods
    void OnAwake() override;
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnDestroy() override;

    // Reflected properties (Proxy)
    RTBEngine::ECS::GameObject* targetRef = nullptr;

    RTB_COMPONENT(Connector)

};
