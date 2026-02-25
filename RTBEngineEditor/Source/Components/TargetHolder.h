#pragma once
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEditor {

    class TargetHolder : public RTBEngine::ECS::Component {
    public:
        TargetHolder();
        ~TargetHolder() override;

        TargetHolder(const TargetHolder&) = delete;
        TargetHolder& operator=(const TargetHolder&) = delete;

        void OnAwake() override {}
        void OnStart() override {}
        void OnUpdate(float deltaTime) override {}
        void OnFixedUpdate(float fixedDeltaTime) override {}
        void OnDestroy() override {}

        // Reflected properties (Proxy)
        RTBEngine::ECS::GameObject* targetRef = nullptr;

        RTB_COMPONENT(TargetHolder)
    };

}
