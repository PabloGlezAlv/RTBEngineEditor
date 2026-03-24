#include "ButtonStyle.h"
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/UI/Elements/UIButton.h>
#include <algorithm>

using ThisClass = ButtonStyle;

RTB_REGISTER_COMPONENT(ButtonStyle)
    RTB_PROPERTY_COMPONENT(backgroundPanel, UIPanel)
    RTB_PROPERTY_COMPONENT(label, UIText)
    RTB_PROPERTY_COLOR(normalPanelColor)
    RTB_PROPERTY_COLOR(normalTextColor)
    RTB_PROPERTY_COLOR(hoverPanelColor)
    RTB_PROPERTY_COLOR(hoverTextColor)
    RTB_PROPERTY_RANGE(hoverScaleBoost,  0.5f, 2.0f)
    RTB_PROPERTY_RANGE(hoverRotationDeg, 0.0f, 45.0f)
    RTB_PROPERTY_COLOR(clickPanelColor)
    RTB_PROPERTY_COLOR(clickTextColor)
    RTB_PROPERTY_RANGE(clickScaleBoost,  0.5f, 2.0f)
    RTB_PROPERTY_RANGE(animationTimeSec, 0.05f, 3.0f)
RTB_END_REGISTER(ButtonStyle)

void ButtonStyle::OnAwake()
{
    currentState      = State::Normal;
    baseScale         = RTBEngine::Math::Vector2(1.0f, 1.0f);
    baseRotation      = 0.0f;
    currentScale      = baseScale;
    currentRotation   = baseRotation;
    currentPanelColor = normalPanelColor;
    currentTextColor  = normalTextColor;
    baseTransformCaptured = false;
    defaultUIButtonVisualsDisabled = false;
    warnedMissingPanel = false;
}

void ButtonStyle::OnStart()
{
    RefreshBindings();
    CaptureBaseVisualTransform();
    if (!backgroundPanel && !warnedMissingPanel) {
        RTB_WARN("ButtonStyle: missing backgroundPanel reference; scale and rotation animation are disabled.");
        warnedMissingPanel = true;
    }
    ApplyCurrentVisuals();
}

void ButtonStyle::OnValidate()
{
    RefreshBindings();
    CaptureBaseVisualTransform();
    ApplyCurrentVisuals();
}

void ButtonStyle::RefreshBindings()
{
    auto* go = GetOwner();
    if (!go) {
        return;
    }

    if (!backgroundPanel) {
        for (const auto& comp : go->GetComponents()) {
            if (!comp) continue;
            if (std::string(comp->GetTypeName()) == "UIPanel") {
                backgroundPanel = static_cast<RTBEngine::UI::UIPanel*>(comp->GetActualObject());
                break;
            }
        }
    }

    if (!label) {
        for (auto* child : go->GetChildren()) {
            if (!child) continue;
            for (const auto& comp : child->GetComponents()) {
                if (!comp) continue;
                if (std::string(comp->GetTypeName()) == "UIText") {
                    label = static_cast<RTBEngine::UI::UIText*>(comp->GetActualObject());
                    break;
                }
            }
            if (label) {
                break;
            }
        }
    }

    if (!defaultUIButtonVisualsDisabled) {
        for (const auto& comp : go->GetComponents()) {
            if (!comp) continue;
            if (std::string(comp->GetTypeName()) == "UIButton") {
                auto* button = static_cast<RTBEngine::UI::UIButton*>(comp->GetActualObject());
                if (button) {
                    button->enableDefaultHoverVisuals = false;
                }
                defaultUIButtonVisualsDisabled = true;
                break;
            }
        }
    }

    if (backgroundPanel) {
        warnedMissingPanel = false;
    }
}

void ButtonStyle::CaptureBaseVisualTransform()
{
    if (baseTransformCaptured || !backgroundPanel) {
        return;
    }

    baseScale = backgroundPanel->scale;
    baseRotation = backgroundPanel->rotation;
    currentScale = baseScale;
    currentRotation = baseRotation;
    baseTransformCaptured = true;
}

void ButtonStyle::ApplyCurrentVisuals()
{
    if (backgroundPanel) {
        backgroundPanel->backgroundColor = currentPanelColor;
        backgroundPanel->scale = currentScale;
        backgroundPanel->rotation = currentRotation;
    }

    if (label) {
        label->color = currentTextColor;
    }
}

void ButtonStyle::SetState(State nextState)
{
    currentState = nextState;
}

void ButtonStyle::OnUpdate(float deltaTime)
{
    RefreshBindings();
    CaptureBaseVisualTransform();
    if (!backgroundPanel && !warnedMissingPanel) {
        RTB_WARN("ButtonStyle: missing backgroundPanel reference; scale and rotation animation are disabled.");
        warnedMissingPanel = true;
    }

    RTBEngine::Math::Vector2 targetScale;
    float                    targetRotation;
    RTBEngine::Math::Vector4 targetPanelColor;
    RTBEngine::Math::Vector4 targetTextColor;

    switch (currentState)
    {
    case State::Hover:
        targetScale      = RTBEngine::Math::Vector2(baseScale.x * hoverScaleBoost, baseScale.y * hoverScaleBoost);
        targetRotation   = baseRotation + hoverRotationDeg;
        targetPanelColor = hoverPanelColor;
        targetTextColor  = hoverTextColor;
        break;
    case State::Pressed:
        targetScale      = RTBEngine::Math::Vector2(baseScale.x * clickScaleBoost, baseScale.y * clickScaleBoost);
        targetRotation   = baseRotation + hoverRotationDeg;
        targetPanelColor = clickPanelColor;
        targetTextColor  = clickTextColor;
        break;
    default:
        targetScale      = baseScale;
        targetRotation   = baseRotation;
        targetPanelColor = normalPanelColor;
        targetTextColor  = normalTextColor;
        break;
    }

    const float safeAnimationTime = std::max(0.001f, animationTimeSec);
    float t = std::min(1.0f, deltaTime / safeAnimationTime);
    currentScale      = LerpV2(currentScale,      targetScale,      t);
    currentRotation   = LerpF (currentRotation,   targetRotation,   t);
    currentPanelColor = LerpV4(currentPanelColor, targetPanelColor, t);
    currentTextColor  = LerpV4(currentTextColor,  targetTextColor,  t);

    ApplyCurrentVisuals();
}

void ButtonStyle::OnPointerEnter(const RTBEngine::UI::PointerEventData&)
{
    SetState(State::Hover);
}

void ButtonStyle::OnPointerExit(const RTBEngine::UI::PointerEventData&)
{
    SetState(State::Normal);
}

void ButtonStyle::OnPointerDown(const RTBEngine::UI::PointerEventData&)
{
    SetState(State::Pressed);
}

void ButtonStyle::OnPointerUp(const RTBEngine::UI::PointerEventData&)
{
    SetState(State::Hover);
}

void ButtonStyle::OnPointerClick(const RTBEngine::UI::PointerEventData&)
{
}

float ButtonStyle::LerpF(float a, float b, float t)
{
    return a + (b - a) * t;
}
RTBEngine::Math::Vector2 ButtonStyle::LerpV2(const RTBEngine::Math::Vector2& a, const RTBEngine::Math::Vector2& b, float t)
{
    return RTBEngine::Math::Vector2(LerpF(a.x, b.x, t), LerpF(a.y, b.y, t));
}
RTBEngine::Math::Vector4 ButtonStyle::LerpV4(const RTBEngine::Math::Vector4& a, const RTBEngine::Math::Vector4& b, float t)
{
    return RTBEngine::Math::Vector4(LerpF(a.x, b.x, t), LerpF(a.y, b.y, t), LerpF(a.z, b.z, t), LerpF(a.w, b.w, t));
}
