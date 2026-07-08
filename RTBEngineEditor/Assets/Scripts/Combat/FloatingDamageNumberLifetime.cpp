#include "FloatingDamageNumberLifetime.h"

#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Math/Matrix/Matrix4.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Scene/CameraComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/ObjectPool.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/UI/Canvas.h>
#include <RTBEngine/UI/Elements/UIText.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

using ThisClass = FloatingDamageNumberLifetime;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;
}

RTB_REGISTER_COMPONENT(FloatingDamageNumberLifetime)
    RTB_PROPERTY_RANGE(lifetime, 0.1f, 3.0f)
    RTB_PROPERTY_RANGE(popDuration, 0.02f, 1.0f)
    RTB_PROPERTY_RANGE(fadeStartNormalized, 0.0f, 1.0f)
    RTB_PROPERTY_RANGE(riseDistance, 0.0f, 5.0f)
    RTB_PROPERTY_RANGE(startScale, 0.01f, 2.0f)
    RTB_PROPERTY_RANGE(holdScale, 0.25f, 3.0f)
    RTB_PROPERTY_COLOR(textColor)
RTB_END_REGISTER(FloatingDamageNumberLifetime)

void FloatingDamageNumberLifetime::OnPoolAcquire()
{
    elapsed = 0.0f;
    isPlaying = false;
}

void FloatingDamageNumberLifetime::OnPoolRelease()
{
    elapsed = 0.0f;
    isPlaying = false;

    if (damageText) {
        damageText->SetVisible(false);
    }
}

void FloatingDamageNumberLifetime::ResolveDamageText()
{
    if (damageText || !owner) {
        return;
    }

    damageText = owner->GetComponentInChildren<RTBEngine::UI::UIText>();
}

void FloatingDamageNumberLifetime::ResolveDamageCanvas()
{
    if (damageCanvas || !owner) {
        return;
    }

    damageCanvas = owner->GetComponentInChildren<RTBEngine::UI::Canvas>();
}

void FloatingDamageNumberLifetime::EnsureWorldSpaceBillboard()
{
    ResolveDamageCanvas();
    if (!damageCanvas) {
        return;
    }

    damageCanvas->SetRenderMode(RTBEngine::UI::Canvas::RenderMode::WorldSpace);
    damageCanvas->SetFaceCamera(true);
    damageCanvas->MarkHierarchyDirty();

    RTBEngine::ECS::GameObject* canvasObject = damageCanvas->GetOwner();
    if (!canvasObject) {
        return;
    }

    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

    RTBEngine::ECS::CameraComponent* cameraComponent = scene->GetMainCamera();
    if (!cameraComponent) {
        return;
    }

    RTBEngine::Rendering::Camera* camera = cameraComponent->GetCamera();
    if (!camera) {
        return;
    }

    const RTBEngine::Math::Vector3 worldPosition = canvasObject->GetWorldPosition();
    RTBEngine::Math::Vector3 forward = camera->GetPosition() - worldPosition;
    if (forward.LengthSquared() <= kDirectionEpsilon) {
        forward = camera->GetForward();
    } else {
        forward.Normalize();
    }

    RTBEngine::Math::Vector3 upReference = RTBEngine::Math::Vector3::Up();
    RTBEngine::Math::Vector3 right = upReference.Cross(forward);
    if (right.LengthSquared() <= kDirectionEpsilon) {
        upReference = RTBEngine::Math::Vector3::Forward();
        right = upReference.Cross(forward);
    }
    right.Normalize();

    const RTBEngine::Math::Vector3 up = forward.Cross(right).Normalized();

    RTBEngine::Math::Matrix4 rotation = RTBEngine::Math::Matrix4::Identity();
    rotation.m[0] = right.x;
    rotation.m[4] = right.y;
    rotation.m[8] = right.z;
    rotation.m[1] = up.x;
    rotation.m[5] = up.y;
    rotation.m[9] = up.z;
    rotation.m[2] = forward.x;
    rotation.m[6] = forward.y;
    rotation.m[10] = forward.z;

    canvasObject->GetTransform().SetRotation(RTBEngine::Math::Quaternion::FromMatrix(rotation));
}

void FloatingDamageNumberLifetime::Play(float damageAmount, const RTBEngine::Math::Vector3& worldPosition)
{
    if (!owner || damageAmount <= 0.0f) {
        return;
    }

    ResolveDamageText();
    if (!damageText) {
        return;
    }

    const int roundedDamage = static_cast<int>(std::lround(damageAmount));
    damageText->SetText(std::to_string(std::max(1, roundedDamage)));

    baseTextColor = textColor;
    damageText->SetColor(baseTextColor);
    damageText->SetVisible(true);
    damageText->SetScale(RTBEngine::Math::Vector2(startScale, startScale));

    spawnWorldPosition = worldPosition;
    driftOffset = RTBEngine::Math::Vector3(
        static_cast<float>((std::rand() % 41) - 20) * 0.012f,
        static_cast<float>((std::rand() % 21)) * 0.008f,
        static_cast<float>((std::rand() % 41) - 20) * 0.012f);

    owner->GetTransform().SetPosition(spawnWorldPosition + driftOffset);
    EnsureWorldSpaceBillboard();

    elapsed = 0.0f;
    isPlaying = true;
}

void FloatingDamageNumberLifetime::OnUpdate(float deltaTime)
{
    if (!isPlaying || !owner) {
        return;
    }

    ResolveDamageText();
    if (!damageText) {
        Finish();
        return;
    }

    elapsed += std::max(0.0f, deltaTime);
    const float totalDuration = std::max(0.01f, lifetime);
    const float normalizedTime = RTBEngine::Math::Clamp01(elapsed / totalDuration);

    const float rise = riseDistance * RTBEngine::Math::EaseOutCubic(normalizedTime);
    owner->GetTransform().SetPosition(spawnWorldPosition + driftOffset + RTBEngine::Math::Vector3(0.0f, rise, 0.0f));
    EnsureWorldSpaceBillboard();

    float scale = holdScale;
    if (elapsed <= popDuration) {
        const float popNormalized = RTBEngine::Math::Clamp01(elapsed / std::max(0.01f, popDuration));
        scale = RTBEngine::Math::Lerp(startScale, holdScale, RTBEngine::Math::EaseOutCubic(popNormalized));
    }

    float alpha = 1.0f;
    if (normalizedTime >= fadeStartNormalized) {
        const float fadeNormalized =
            RTBEngine::Math::Clamp01((normalizedTime - fadeStartNormalized) / std::max(0.01f, 1.0f - fadeStartNormalized));
        alpha = 1.0f - fadeNormalized;
    }

    damageText->SetScale(RTBEngine::Math::Vector2(scale, scale));
    damageText->SetColor(RTBEngine::Math::Vector4(
        baseTextColor.x,
        baseTextColor.y,
        baseTextColor.z,
        baseTextColor.w * alpha));

    if (elapsed >= totalDuration) {
        Finish();
    }
}

void FloatingDamageNumberLifetime::Finish()
{
    isPlaying = false;

    if (owner) {
        RTBEngine::ECS::ObjectPool::GetInstance().Release(owner);
    }
}
