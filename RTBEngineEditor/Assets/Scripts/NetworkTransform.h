#pragma once

#include "OnlineGameplayNet.h"

#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

class NetworkTransform : public RTBEngine::ECS::Component {
public:
    NetworkTransform() = default;
    ~NetworkTransform() override = default;

    std::string objectKey;  // optional override; usually resolved from NetworkIdentity
    float sendRate = 20.0f;
    float interpolationSpeed = 14.0f;
    bool replicatePosition = true;
    bool replicateRotation = true;

    RTB_COMPONENT(NetworkTransform)

public:
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnLateUpdate(float deltaTime) override;
    void OnValidate() override;

private:
    float sendTimer = 0.0f;
    std::string resolvedObjectKey;
    bool hasCachedSnapshot = false;
    OnlineGameplayNet::TransformSnapshot cachedSnapshot;

    void ResolveObjectKey();
    bool HasSendAuthority() const;     // true on host (or offline)
    bool HasReceiveAuthority() const;  // true on clients in online lobby
    void SendSnapshot(float deltaTime);
    void ApplyRemoteSnapshot(float deltaTime);
};
