#pragma once

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

class NetworkTransform : public RTBEngine::ECS::Component {
public:
    NetworkTransform() = default;
    ~NetworkTransform() override = default;

    std::string objectKey;
    float sendRate = 15.0f;
    float interpolationSpeed = 14.0f;
    bool replicatePosition = true;
    bool replicateRotation = true;

    RTB_COMPONENT(NetworkTransform)

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnValidate() override;

private:
    float sendTimer = 0.0f;
    std::string resolvedObjectKey;

    void ResolveObjectKey();
    bool HasLocalAuthority() const;
    void SendSnapshot(float deltaTime);
    void ApplyRemoteSnapshot(float deltaTime);
};
