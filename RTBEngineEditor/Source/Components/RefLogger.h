#pragma once
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include "TargetHolder.h"

namespace RTBEditor {

    class RefLogger : public RTBEngine::ECS::Component {
    public:
        RefLogger();
        ~RefLogger() override;

        RefLogger(const RefLogger&) = delete;
        RefLogger& operator=(const RefLogger&) = delete;

        void OnAwake() override {}
        void OnStart() override {}
        void OnUpdate(float deltaTime) override {}
        void OnFixedUpdate(float fixedDeltaTime) override {}
        void OnDestroy() override {}

        // Reflected properties (Proxy)
        TargetHolder* holderRef = nullptr;

        RTB_COMPONENT(RefLogger)
    };

}
