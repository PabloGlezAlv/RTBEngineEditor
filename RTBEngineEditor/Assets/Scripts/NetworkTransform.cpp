#include "NetworkTransform.h"

#include "OnlineGameplayNet.h"

#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/RigidBodyComponent.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Math/Vectors/Vector3.h>

#include <algorithm>

using ThisClass = NetworkTransform;

RTB_REGISTER_COMPONENT(NetworkTransform)
    RTB_PROPERTY(objectKey)
    RTB_PROPERTY_RANGE(sendRate, 1.0f, 60.0f)
    RTB_PROPERTY_RANGE(interpolationSpeed, 1.0f, 60.0f)
    RTB_PROPERTY(replicatePosition)
    RTB_PROPERTY(replicateRotation)
RTB_END_REGISTER(NetworkTransform)

namespace {

    RTBEngine::Math::Vector3 LerpVector(
        const RTBEngine::Math::Vector3& from,
        const RTBEngine::Math::Vector3& to,
        float t)
    {
        return from + (to - from) * t;
    }

    void SyncRigidBodyIfPresent(
        RTBEngine::ECS::GameObject* owner,
        const RTBEngine::Math::Vector3& position,
        const RTBEngine::Math::Quaternion& rotation)
    {
        if (!owner) {
            return;
        }

        auto* rigidBodyComponent = owner->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
        if (!rigidBodyComponent || !rigidBodyComponent->HasRigidBody() || !rigidBodyComponent->GetRigidBody()) {
            return;
        }

        rigidBodyComponent->GetRigidBody()->SetWorldTransform(position, rotation);
    }

}

void NetworkTransform::OnStart()
{
    SetUpdateTickEnabled(true);
    ResolveObjectKey();
    OnValidate();
}

void NetworkTransform::OnUpdate(float deltaTime)
{
    if (!owner || !OnlineGameplayNet::IsInOnlineLobby()) {
        return;
    }

    ResolveObjectKey();
    OnlineGameplayNet::Pump();

    if (HasLocalAuthority()) {
        SendSnapshot(deltaTime);
    } else {
        ApplyRemoteSnapshot(deltaTime);
    }
}

void NetworkTransform::OnValidate()
{
    sendRate = std::clamp(sendRate, 1.0f, 60.0f);
    interpolationSpeed = std::clamp(interpolationSpeed, 1.0f, 60.0f);
    ResolveObjectKey();
}

void NetworkTransform::ResolveObjectKey()
{
    if (!owner) {
        resolvedObjectKey = objectKey;
        return;
    }

    if (!objectKey.empty()) {
        resolvedObjectKey = objectKey;
        return;
    }

    resolvedObjectKey = owner->GetUUID().empty()
        ? owner->GetName()
        : owner->GetUUID();
}

bool NetworkTransform::HasLocalAuthority() const
{
    // V1 follows the current architecture: the lobby owner is the authoritative host.
    return !OnlineGameplayNet::IsInOnlineLobby() || OnlineGameplayNet::IsLobbyOwner();
}

void NetworkTransform::SendSnapshot(float deltaTime)
{
    if (resolvedObjectKey.empty()) {
        return;
    }

    sendTimer += std::max(0.0f, deltaTime);
    const float sendInterval = 1.0f / std::max(1.0f, sendRate);
    if (sendTimer < sendInterval) {
        return;
    }

    sendTimer = 0.0f;

    OnlineGameplayNet::TransformSnapshot snapshot;
    snapshot.objectKey = resolvedObjectKey;
    snapshot.position = owner->GetTransform().GetPosition();
    snapshot.rotation = owner->GetTransform().GetRotation();
    OnlineGameplayNet::BroadcastTransform(snapshot);
}

void NetworkTransform::ApplyRemoteSnapshot(float deltaTime)
{
    if (resolvedObjectKey.empty()) {
        return;
    }

    OnlineGameplayNet::TransformSnapshot snapshot;
    if (!OnlineGameplayNet::ConsumeLatestTransform(resolvedObjectKey, snapshot)) {
        return;
    }

    const float t = std::clamp(interpolationSpeed * std::max(0.0f, deltaTime), 0.0f, 1.0f);
    RTBEngine::Math::Vector3 nextPosition = owner->GetTransform().GetPosition();
    RTBEngine::Math::Quaternion nextRotation = owner->GetTransform().GetRotation();

    if (replicatePosition) {
        nextPosition = LerpVector(nextPosition, snapshot.position, t);
        owner->GetTransform().SetPosition(nextPosition);
    }

    if (replicateRotation) {
        nextRotation = RTBEngine::Math::Quaternion::Slerp(nextRotation, snapshot.rotation, t);
        owner->GetTransform().SetRotation(nextRotation);
    }

    SyncRigidBodyIfPresent(owner, nextPosition, nextRotation);
}
