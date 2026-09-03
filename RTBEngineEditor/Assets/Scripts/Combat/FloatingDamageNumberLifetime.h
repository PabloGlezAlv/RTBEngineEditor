#pragma once

#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

namespace RTBEngine {
    namespace UI {
        class Canvas;
        class UIText;
    }
}

class FloatingDamageNumberLifetime : public RTBEngine::Scene::Component
{
public:
    FloatingDamageNumberLifetime() = default;
    ~FloatingDamageNumberLifetime() override = default;

    float lifetime = 1.06f;
    float popDuration = 0.12f;
    float fadeStartNormalized = 0.55f;
    float riseDistance = 0.85f;
    float startScale = 0.35f;
    float holdScale = 1.0f;
    RTBEngine::Math::Vector4 textColor = RTBEngine::Math::Vector4(1.0f, 0.92f, 0.35f, 1.0f);

    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate(float deltaTime) override;
    void Play(float damageAmount, const RTBEngine::Math::Vector3& worldPosition);

    RTB_COMPONENT(FloatingDamageNumberLifetime)

private:
    RTBEngine::UI::UIText* damageText = nullptr;
    RTBEngine::UI::Canvas* damageCanvas = nullptr;
    RTBEngine::Math::Vector3 spawnWorldPosition = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector3 driftOffset = RTBEngine::Math::Vector3::Zero();
    RTBEngine::Math::Vector4 baseTextColor = RTBEngine::Math::Vector4(1.0f, 0.92f, 0.35f, 1.0f);
    float elapsed = 0.0f;
    bool isPlaying = false;

    void ResolveDamageText();
    void ResolveDamageCanvas();
    void EnsureWorldSpaceBillboard();
    void Finish();
};
