#pragma once
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class NewComponent : public RTBEngine::ECS::Component {
public:
    NewComponent();
    ~NewComponent() override;

    NewComponent(const NewComponent&) = delete;
    NewComponent& operator=(const NewComponent&) = delete;

    //Loop methods
    void OnAwake() override;
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnDestroy() override;

    // Reflected properties (Proxy)
    float speedRef = 1.0f;

    RTB_COMPONENT(NewComponent)

private:
    float speed = 1.0f;
};
