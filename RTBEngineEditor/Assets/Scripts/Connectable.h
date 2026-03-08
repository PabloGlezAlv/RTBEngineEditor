#pragma once
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class Connectable : public RTBEngine::ECS::Component {
public:
    Connectable();
    ~Connectable() override;

    Connectable(const Connectable&) = delete;
    Connectable& operator=(const Connectable&) = delete;

    //Loop methods
    void OnAwake() override;
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnDestroy() override;

    RTB_COMPONENT(Connectable)
};
