#include "ButtonStyle.h"
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/UI/UIElement.h>
#include <algorithm>
#include <cmath>

// ============================================================================
//  ButtonStyle — registration & implementation
// ============================================================================
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
    if (auto* go = GetOwner()) {
        // Avoid RTTI/dynamic_cast across module boundaries when resolving engine components.
        for (const auto& comp : go->GetComponents()) {
            if (!comp) continue;
            if (std::string(comp->GetTypeName()) == "UIElement") {
                auto* uiEl = static_cast<RTBEngine::UI::UIElement*>(comp->GetActualObject());
                if (uiEl) {
                    baseScale = uiEl->scale;
                    baseRotation = uiEl->rotation;
                }
                break;
            }
        }
    }
    currentScale      = baseScale;
    currentRotation   = baseRotation;
    currentPanelColor = normalPanelColor;
    currentTextColor  = normalTextColor;

    RTB_INFO(
        "ButtonStyle::OnAwake - panel=" + std::string(backgroundPanel ? "OK" : "NULL") +
        " label=" + std::string(label ? "OK" : "NULL") +
        " baseScale=(" + std::to_string(baseScale.x) + "," + std::to_string(baseScale.y) + ")" +
        " animTimeSec=" + std::to_string(animationTimeSec));
}

void ButtonStyle::OnUpdate(float deltaTime)
{
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

    if (backgroundPanel)
    {
        backgroundPanel->backgroundColor = currentPanelColor;
        backgroundPanel->scale           = currentScale;
        backgroundPanel->rotation        = currentRotation;
    }
    if (label)
    {
        // The label is usually a child of the panel and already follows panel transform.
        // Only animate text color here to avoid double-scaling/rotating.
        label->color = currentTextColor;
    }
}

void ButtonStyle::NotifyEnter()
{
    currentState = State::Hover;
    RTB_INFO("ButtonStyle::NotifyEnter");
}
void ButtonStyle::NotifyExit()
{
    currentState = State::Normal;
    RTB_INFO("ButtonStyle::NotifyExit");
}
void ButtonStyle::NotifyDown()
{
    currentState = State::Pressed;
    RTB_INFO("ButtonStyle::NotifyDown");
}
void ButtonStyle::NotifyUp()
{
    currentState = State::Normal;
    RTB_INFO("ButtonStyle::NotifyUp");
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

// ============================================================================
//  ButtonStyleEvents — registration & implementation
// ============================================================================
// No reflected properties — just the component boilerplate
RTB_REGISTER_COMPONENT(ButtonStyleEvents)
RTB_END_REGISTER(ButtonStyleEvents)

void ButtonStyleEvents::OnAwake()
{
    style = nullptr;
    auto* go = GetOwner();
    if (!go) return;

    // Avoid RTTI/dynamic_cast across module boundaries when resolving sibling scripts.
    for (const auto& comp : go->GetComponents()) {
        if (!comp) continue;
        if (std::string(comp->GetTypeName()) == "ButtonStyle") {
            style = static_cast<ButtonStyle*>(comp->GetActualObject());
            break;
        }
    }

    RTB_INFO("ButtonStyleEvents::OnAwake - style=" + std::string(style ? "OK" : "NULL"));
}

void ButtonStyleEvents::OnPointerEnter(const RTBEngine::UI::PointerEventData&)
{
    RTB_INFO("ButtonStyleEvents::OnPointerEnter");
    if (style) style->NotifyEnter();
}
void ButtonStyleEvents::OnPointerExit (const RTBEngine::UI::PointerEventData&)
{
    RTB_INFO("ButtonStyleEvents::OnPointerExit");
    if (style) style->NotifyExit();
}
void ButtonStyleEvents::OnPointerDown (const RTBEngine::UI::PointerEventData&)
{
    RTB_INFO("ButtonStyleEvents::OnPointerDown");
    if (style) style->NotifyDown();
}
void ButtonStyleEvents::OnPointerUp   (const RTBEngine::UI::PointerEventData&)
{
    RTB_INFO("ButtonStyleEvents::OnPointerUp");
    if (style) style->NotifyUp();
}
void ButtonStyleEvents::OnPointerClick(const RTBEngine::UI::PointerEventData&) { /* extensible */ }
