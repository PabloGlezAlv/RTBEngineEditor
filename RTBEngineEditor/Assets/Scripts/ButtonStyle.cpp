#include "ButtonStyle.h"
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/UI/Elements/UIButton.h>
#include <algorithm>

using ThisClass = ButtonStyle;

namespace {
    constexpr float kDefaultHoverInTimeSec = 0.12f;
    constexpr float kDefaultHoverOutTimeSec = 0.18f;
    constexpr float kDefaultPressInTimeSec = 0.08f;
    constexpr float kDefaultPressOutTimeSec = 0.12f;
}

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
    RTB_PROPERTY_RANGE(hoverInTimeSec,  0.0f, 3.0f)
    RTB_PROPERTY_RANGE(hoverOutTimeSec, 0.0f, 3.0f)
    RTB_PROPERTY_RANGE(pressInTimeSec,  0.0f, 3.0f)
    RTB_PROPERTY_RANGE(pressOutTimeSec, 0.0f, 3.0f)
RTB_END_REGISTER(ButtonStyle)

void ButtonStyle::OnAwake()
{
    SetTimeMode(RTBEngine::ECS::ComponentTimeMode::Unscaled);
    currentState = State::Normal;
    baseScale = RTBEngine::Math::Vector2(1.0f, 1.0f);
    baseRotation = 0.0f;
    currentScale = baseScale;
    currentRotation = baseRotation;
    currentPanelColor = normalPanelColor;
    currentTextColor = normalTextColor;
    baseTransformCaptured = false;
    defaultUIButtonVisualsDisabled = false;
    warnedMissingPanel = false;
    warnedMissingLabel = false;
    isPointerOver = false;
    isPressed = false;
    activeTransition = {};
    SetUpdateTickEnabled(false);
}

void ButtonStyle::OnStart()
{
    StopTransition();
    NormalizeTimingProperties();
    RefreshBindings();
    baseTransformCaptured = false;
    CaptureBaseVisualTransform();
    isPointerOver = false;
    isPressed = false;
    SetState(State::Normal);
    ResolveTargetVisuals(currentState, currentScale, currentRotation, currentPanelColor, currentTextColor);
    ApplyCurrentVisuals();
    SetUpdateTickEnabled(false);
}

void ButtonStyle::OnValidate()
{
    StopTransition();
    NormalizeTimingProperties();
    RefreshBindings();
    baseTransformCaptured = false;
    CaptureBaseVisualTransform();
    isPointerOver = false;
    isPressed = false;
    SetState(State::Normal);
    ResolveTargetVisuals(currentState, currentScale, currentRotation, currentPanelColor, currentTextColor);
    ApplyCurrentVisuals();
    SetUpdateTickEnabled(false);
}

void ButtonStyle::RefreshBindings()
{
    auto* go = GetOwner();
    if (!go) {
        return;
    }

    if (!backgroundPanel) {
        backgroundPanel = go->GetComponent<RTBEngine::UI::UIPanel>();
    }

    if (!label) {
        label = go->GetComponent<RTBEngine::UI::UIText>();
    }

    if (!defaultUIButtonVisualsDisabled) {
        auto* button = go->GetComponent<RTBEngine::UI::UIButton>();
        if (button) {
            button->enableDefaultHoverVisuals = false;
            defaultUIButtonVisualsDisabled = true;
        }
    }

    if (backgroundPanel) {
        warnedMissingPanel = false;
    } else if (!warnedMissingPanel) {
        RTB_WARN("ButtonStyle: missing backgroundPanel reference; panel visuals are disabled.");
        warnedMissingPanel = true;
    }

    if (label) {
        warnedMissingLabel = false;
    } else if (!warnedMissingLabel) {
        RTB_WARN("ButtonStyle: missing label reference; text color animation is disabled.");
        warnedMissingLabel = true;
    }
}

void ButtonStyle::NormalizeTimingProperties()
{
    if (hoverInTimeSec < 0.0f) {
        hoverInTimeSec = kDefaultHoverInTimeSec;
    }

    if (hoverOutTimeSec < 0.0f) {
        hoverOutTimeSec = kDefaultHoverOutTimeSec;
    }

    if (pressInTimeSec < 0.0f) {
        pressInTimeSec = kDefaultPressInTimeSec;
    }

    if (pressOutTimeSec < 0.0f) {
        pressOutTimeSec = kDefaultPressOutTimeSec;
    }
}

void ButtonStyle::CaptureBaseVisualTransform()
{
    if (baseTransformCaptured || !backgroundPanel) {
        return;
    }

    baseScale = backgroundPanel->GetScale();
    baseRotation = backgroundPanel->GetRotation();
    currentScale = baseScale;
    currentRotation = baseRotation;
    baseTransformCaptured = true;
}

void ButtonStyle::ResolveTargetVisuals(State state,
                                       RTBEngine::Math::Vector2& outScale,
                                       float& outRotation,
                                       RTBEngine::Math::Vector4& outPanelColor,
                                       RTBEngine::Math::Vector4& outTextColor) const
{
    switch (state)
    {
    case State::Hover:
        outScale = RTBEngine::Math::Vector2(baseScale.x * hoverScaleBoost, baseScale.y * hoverScaleBoost);
        outRotation = baseRotation + hoverRotationDeg;
        outPanelColor = hoverPanelColor;
        outTextColor  = hoverTextColor;
        break;
    case State::Pressed:
        outScale = RTBEngine::Math::Vector2(baseScale.x * clickScaleBoost, baseScale.y * clickScaleBoost);
        outRotation = baseRotation;
        outPanelColor = clickPanelColor;
        outTextColor  = clickTextColor;
        break;
    default:
        outScale = baseScale;
        outRotation = baseRotation;
        outPanelColor = normalPanelColor;
        outTextColor  = normalTextColor;
        break;
    }
}

ButtonStyle::State ButtonStyle::ResolveStateFromInteraction() const
{
    if (isPressed && isPointerOver) {
        return State::Pressed;
    }

    if (isPointerOver) {
        return State::Hover;
    }

    return State::Normal;
}

float ButtonStyle::ResolveTransitionDuration(State fromState, State toState) const
{
    if (fromState == toState) {
        return 0.0f;
    }

    switch (toState)
    {
    case State::Hover:
        return (fromState == State::Pressed) ? pressOutTimeSec : hoverInTimeSec;
    case State::Pressed:
        return pressInTimeSec;
    case State::Normal:
    default:
        return (fromState == State::Pressed) ? pressOutTimeSec : hoverOutTimeSec;
    }
}

float ButtonStyle::EaseTransitionProgress(State fromState, State toState, float t) const
{
    if (fromState == State::Pressed && toState == State::Hover) {
        return RTBEngine::Math::EaseOutCubic(t);
    }

    if (toState == State::Normal) {
        return RTBEngine::Math::EaseInOutCubic(t);
    }

    return RTBEngine::Math::EaseOutCubic(t);
}

void ButtonStyle::ApplyCurrentVisuals()
{
    if (backgroundPanel) {
        backgroundPanel->SetBackgroundColor(currentPanelColor);
        backgroundPanel->SetScale(currentScale);
        backgroundPanel->SetRotation(currentRotation);
    }
    if (label) {
        label->SetColor(currentTextColor);
    }
}

void ButtonStyle::SetState(State nextState)
{
    currentState = nextState;
}

void ButtonStyle::OnUpdate(float deltaTime)
{
    latentRunner.Tick(deltaTime);

    if (!latentRunner.HasActiveActions()) {
        activeTransition = {};
        SetUpdateTickEnabled(false);
    }
}

void ButtonStyle::OnDestroy()
{
    StopTransition();
    SetState(State::Normal);
    currentScale = baseScale;
    currentRotation = baseRotation;
    currentPanelColor = normalPanelColor;
    currentTextColor = normalTextColor;
    ApplyCurrentVisuals();
    SetUpdateTickEnabled(false);
}

void ButtonStyle::OnPointerEnter(const RTBEngine::UI::PointerEventData&)
{
    isPointerOver = true;
    StartTransition(ResolveStateFromInteraction());
}

void ButtonStyle::OnPointerExit(const RTBEngine::UI::PointerEventData&)
{
    isPointerOver = false;
    StartTransition(ResolveStateFromInteraction());
}

void ButtonStyle::OnPointerDown(const RTBEngine::UI::PointerEventData&)
{
    isPointerOver = true;
    isPressed = true;
    StartTransition(ResolveStateFromInteraction());
}

void ButtonStyle::OnPointerUp(const RTBEngine::UI::PointerEventData&)
{
    isPressed = false;
    StartTransition(ResolveStateFromInteraction());
}

void ButtonStyle::OnPointerClick(const RTBEngine::UI::PointerEventData&)
{
}

void ButtonStyle::StartTransition(State nextState)
{
    NormalizeTimingProperties();
    CaptureBaseVisualTransform();

    const RTBEngine::Math::Vector2 startScale = currentScale;
    const float startRotation = currentRotation;
    const RTBEngine::Math::Vector4 startPanelColor = currentPanelColor;
    const RTBEngine::Math::Vector4 startTextColor  = currentTextColor;

    const State previousState = currentState;
    SetState(nextState);

    RTBEngine::Math::Vector2 targetScale;
    float targetRotation = 0.0f;
    RTBEngine::Math::Vector4 targetPanelColor;
    RTBEngine::Math::Vector4 targetTextColor;
    ResolveTargetVisuals(currentState, targetScale, targetRotation, targetPanelColor, targetTextColor);

    StopTransition();

    const float transitionDuration = ResolveTransitionDuration(previousState, currentState);
    if (transitionDuration <= 0.0f) {
        FinishTransition(targetScale, targetRotation, targetPanelColor, targetTextColor);
        SetUpdateTickEnabled(false);
        return;
    }

    const float safeAnimationTime = std::max(0.001f, transitionDuration);
    activeTransition = latentRunner.Play(
        RTBEngine::Scripting::LatentSequence()
            .Tween(safeAnimationTime,
                [this, previousState, nextState, startScale, startRotation, startPanelColor, startTextColor, targetScale, targetRotation, targetPanelColor, targetTextColor](float t) {
                    const float easedT = EaseTransitionProgress(previousState, nextState, t);
                    currentScale = RTBEngine::Math::Lerp(startScale, targetScale, easedT);
                    currentRotation = RTBEngine::Math::Lerp(startRotation, targetRotation, easedT);
                    currentPanelColor = RTBEngine::Math::Lerp(startPanelColor, targetPanelColor, easedT);
                    currentTextColor  = RTBEngine::Math::Lerp(startTextColor,  targetTextColor,  easedT);
                    ApplyCurrentVisuals();
                },
                [this, targetScale, targetRotation, targetPanelColor, targetTextColor]() {
                    FinishTransition(targetScale, targetRotation, targetPanelColor, targetTextColor);
                })
    );

    if (!activeTransition.IsValid()) {
        FinishTransition(targetScale, targetRotation, targetPanelColor, targetTextColor);
        SetUpdateTickEnabled(false);
        return;
    }

    SetUpdateTickEnabled(true);
}

void ButtonStyle::StopTransition()
{
    if (activeTransition.IsValid()) {
        latentRunner.Stop(activeTransition);
        activeTransition = {};
    } else if (latentRunner.HasActiveActions()) {
        latentRunner.StopAll();
    }
}

void ButtonStyle::FinishTransition(const RTBEngine::Math::Vector2& finalScale,
                                   float finalRotation,
                                   const RTBEngine::Math::Vector4& finalPanelColor,
                                   const RTBEngine::Math::Vector4& finalTextColor)
{
    currentScale = finalScale;
    currentRotation = finalRotation;
    currentPanelColor = finalPanelColor;
    currentTextColor = finalTextColor;
    activeTransition = {};
    ApplyCurrentVisuals();
}
