#include "ComicBubbleLifetime.h"

#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Math/Matrix/Matrix4.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Rendering/Camera.h>
#include <RTBEngine/Scene/CameraComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/UI/Canvas.h>
#include <RTBEngine/UI/Elements/UIImage.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>

using ThisClass = ComicBubbleLifetime;

namespace {
    constexpr float kDirectionEpsilon = 0.0001f;
}

RTB_REGISTER_COMPONENT(ComicBubbleLifetime)
    RTB_PROPERTY_RANGE(growDuration, 0.05f, 3.0f)
    RTB_PROPERTY_RANGE(burstDuration, 0.02f, 1.0f)
    RTB_PROPERTY_RANGE(startScale, 0.01f, 1.0f)
    RTB_PROPERTY_RANGE(holdScale, 0.25f, 3.0f)
    RTB_PROPERTY_RANGE(burstScale, 1.0f, 6.0f)
RTB_END_REGISTER(ComicBubbleLifetime)

void ComicBubbleLifetime::ResolveBubbleImage()
{
    if (bubbleImage) {
        return;
    }

    bubbleImage = owner->GetComponentInChildren<RTBEngine::UI::UIImage>();
}

void ComicBubbleLifetime::ResolveBubbleCanvas()
{
    if (bubbleCanvas) {
        return;
    }

    bubbleCanvas = owner->GetComponentInChildren<RTBEngine::UI::Canvas>();
}

void ComicBubbleLifetime::EnsureWorldSpaceBillboard()
{
    ResolveBubbleCanvas();
    if (!bubbleCanvas) {
        return;
    }

    bubbleCanvas->SetRenderMode(RTBEngine::UI::Canvas::RenderMode::WorldSpace);
    bubbleCanvas->SetFaceCamera(true);
    bubbleCanvas->MarkHierarchyDirty();

    RTBEngine::Scene::GameObject* canvasObject = bubbleCanvas->GetOwner();
    if (!canvasObject) {
        return;
    }

    RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
    if (!scene) {
        return;
    }

    RTBEngine::Scene::CameraComponent* cameraComponent = scene->GetMainCamera();
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

void ComicBubbleLifetime::Play(const std::string& texturePath, const RTBEngine::Math::Vector3& worldPosition)
{
    ResolveBubbleImage();
    if (!bubbleImage) {
        return;
    }

    if (!texturePath.empty()) {
        if (RTBEngine::Rendering::Texture* texture =
                RTBEngine::Core::ResourceManager::GetInstance().LoadTexture(texturePath)) {
            bubbleImage->SetTexture(texture);
        }
    }

    spawnWorldPosition = worldPosition;
    owner->GetTransform().SetPosition(spawnWorldPosition);
    EnsureWorldSpaceBillboard();

    elapsed = 0.0f;
    isPlaying = true;
    tiltDegrees = static_cast<float>((std::rand() % 21) - 10);

    bubbleImage->SetVisible(true);
    bubbleImage->SetTint(RTBEngine::Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    bubbleImage->SetScale(RTBEngine::Math::Vector2(startScale, startScale));
    bubbleImage->SetRotation(tiltDegrees);
}

void ComicBubbleLifetime::OnUpdate(float deltaTime)
{
    if (!isPlaying) {
        return;
    }

    ResolveBubbleImage();
    if (!bubbleImage) {
        Finish();
        return;
    }

    owner->GetTransform().SetPosition(spawnWorldPosition);
    EnsureWorldSpaceBillboard();

    elapsed += std::max(0.0f, deltaTime);
    const float totalDuration = std::max(0.01f, growDuration + burstDuration);

    float scale = holdScale;
    float alpha = 1.0f;

    if (elapsed <= growDuration) {
        const float growNormalized = RTBEngine::Math::Clamp01(elapsed / std::max(0.01f, growDuration));
        scale = RTBEngine::Math::Lerp(startScale, holdScale, RTBEngine::Math::EaseOutCubic(growNormalized));
    } else {
        const float burstElapsed = elapsed - growDuration;
        const float burstNormalized =
            RTBEngine::Math::Clamp01(burstElapsed / std::max(0.01f, burstDuration));
        scale = RTBEngine::Math::Lerp(holdScale, burstScale, RTBEngine::Math::EaseInCubic(burstNormalized));
        alpha = 1.0f - burstNormalized;
    }

    bubbleImage->SetScale(RTBEngine::Math::Vector2(scale, scale));
    bubbleImage->SetTint(RTBEngine::Math::Vector4(1.0f, 1.0f, 1.0f, alpha));

    if (elapsed >= totalDuration) {
        Finish();
    }
}

void ComicBubbleLifetime::Finish()
{
    isPlaying = false;

    RTBEngine::Scene::GameObject* root = owner;
    if (!root) {
        return;
    }

    if (RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene()) {
        scene->RemoveGameObject(root);
    }
}
