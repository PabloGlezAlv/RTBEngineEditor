#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class CursorUnlocker : public RTBEngine::ECS::Component
{
public:
    CursorUnlocker() = default;
    ~CursorUnlocker() override = default;

    void OnStart() override;
    void OnValidate() override;

    RTB_COMPONENT(CursorUnlocker)

private:
    void Apply() const;
};
