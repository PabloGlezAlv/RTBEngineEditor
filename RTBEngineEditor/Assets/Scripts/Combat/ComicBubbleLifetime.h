#pragma once

#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

#include <string>

namespace RTBEngine {
    namespace UI {
        class Canvas;
        class UIImage;
    }
}

class ComicBubbleLifetime : public RTBEngine::Scene::Component
{
public:
    ComicBubbleLifetime() = default;
    ~ComicBubbleLifetime() override = default;

    float growDuration = 0.85f;
    float burstDuration = 0.12f;
    float startScale = 0.15f;
    float holdScale = 1.0f;
    float burstScale = 2.5f;

    RTB_COMPONENT(ComicBubbleLifetime)

public:
    void OnUpdate(float deltaTime) override;

    void Play(const std::string& texturePath, const RTBEngine::Math::Vector3& worldPosition);

private:
    RTBEngine::UI::UIImage* bubbleImage = nullptr;
    RTBEngine::UI::Canvas* bubbleCanvas = nullptr;
    RTBEngine::Math::Vector3 spawnWorldPosition = RTBEngine::Math::Vector3::Zero();
    float elapsed = 0.0f;
    float tiltDegrees = 0.0f;
    bool isPlaying = false;

    void ResolveBubbleImage();
    void ResolveBubbleCanvas();
    void EnsureWorldSpaceBillboard();
    void Finish();
};
