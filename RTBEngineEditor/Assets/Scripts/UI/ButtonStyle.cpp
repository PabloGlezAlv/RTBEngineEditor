#include "ButtonStyle.h"

#include <RTBEngine/Core/Logger.h>

#include <RTBEngine/Scene/AudioSourceComponent.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/UI/Elements/UIButton.h>

#include <RTBEngine/UI/Elements/UIImage.h>

#include <RTBEngine/UI/Elements/UIPanel.h>

#include <algorithm>



using ThisClass = ButtonStyle;



namespace {

    constexpr float kDefaultHoverInTimeSec = 0.12f;

    constexpr float kDefaultHoverOutTimeSec = 0.18f;

    constexpr float kDefaultPressInTimeSec = 0.08f;

    constexpr float kDefaultPressOutTimeSec = 0.12f;

}



RTB_REGISTER_COMPONENT(ButtonStyle)

    RTB_PROPERTY_COMPONENT(backgroundImage, UIImage)

    RTB_PROPERTY_COMPONENT(label, UIText)

    RTB_PROPERTY_TEXTURE(normalTexture)

    RTB_PROPERTY_TEXTURE(hoverTexture)

    RTB_PROPERTY_TEXTURE(pressedTexture)

    RTB_PROPERTY_COLOR(normalTextColor)

    RTB_PROPERTY_COLOR(normalImageTint)

    RTB_PROPERTY_COLOR(hoverTextColor)

    RTB_PROPERTY_COLOR(hoverImageTint)

    RTB_PROPERTY_RANGE(hoverScaleBoost,  0.5f, 2.0f)

    RTB_PROPERTY_RANGE(hoverRotationDeg, 0.0f, 45.0f)

    RTB_PROPERTY_COLOR(clickTextColor)

    RTB_PROPERTY_COLOR(clickImageTint)

    RTB_PROPERTY_RANGE(clickScaleBoost,  0.5f, 2.0f)

    RTB_PROPERTY_COLOR(disabledTextColor)

    RTB_PROPERTY_COLOR(disabledImageTint)

    RTB_PROPERTY_RANGE(hoverInTimeSec,  0.0f, 3.0f)

    RTB_PROPERTY_RANGE(hoverOutTimeSec, 0.0f, 3.0f)

    RTB_PROPERTY_RANGE(pressInTimeSec,  0.0f, 3.0f)

    RTB_PROPERTY_RANGE(pressOutTimeSec, 0.0f, 3.0f)

    RTB_PROPERTY(playClickSound)

    RTB_PROPERTY_COMPONENT(clickAudio, AudioSourceComponent)

RTB_END_REGISTER(ButtonStyle)



void ButtonStyle::OnAwake()

{

    SetTimeMode(RTBEngine::Scene::ComponentTimeMode::Unscaled);

    currentState = State::Normal;

    baseScale = RTBEngine::Math::Vector2(1.0f, 1.0f);

    baseRotation = 0.0f;

    currentScale = baseScale;

    currentRotation = baseRotation;

    currentTextColor = normalTextColor;

    currentImageTint = normalImageTint;

    baseTransformCaptured = false;

    defaultUIButtonVisualsDisabled = false;

    warnedMissingImage = false;

    warnedMissingLabel = false;

    isPointerOver = false;

    isPressed = false;

    lastInteractable = true;

    hitPanel = nullptr;

    hitPanelCaptured = false;

    activeTransition = {};

    SetUpdateTickEnabled(false);

    CaptureHitPanelAppearance();

    RefreshBindings();

}



void ButtonStyle::OnStart()
{
    InitializeVisualState(true);
}

void ButtonStyle::OnValidate()
{
    InitializeVisualState(false);
}

void ButtonStyle::InitializeVisualState(bool enableRuntimePolling)
{
    StopTransition();
    NormalizeTimingProperties();
    RefreshBindings();
    baseTransformCaptured = false;
    CaptureBaseVisualTransform();
    isPointerOver = false;
    isPressed = false;

    lastInteractable = IsButtonInteractable();
    currentState = lastInteractable ? State::Normal : State::Disabled;

    RTBEngine::Math::Vector2 targetScale;
    float targetRotation = 0.0f;
    RTBEngine::Math::Vector4 targetTextColor;
    RTBEngine::Math::Vector4 targetImageTint;
    ResolveTargetVisuals(currentState, targetScale, targetRotation, targetTextColor, targetImageTint);
    ApplyTextureForState(currentState);
    FinishTransition(targetScale, targetRotation, targetTextColor, targetImageTint);
    RestoreHitPanelAppearance();

    if (enableRuntimePolling) {
        RefreshUpdateTick();
    } else {
        SetUpdateTickEnabled(false);
    }
}



void ButtonStyle::CaptureHitPanelAppearance()

{

    if (hitPanelCaptured) {

        return;

    }



    RTBEngine::Scene::GameObject* go = GetOwner();

    if (!go) {

        return;

    }



    hitPanel = go->GetComponent<RTBEngine::UI::UIPanel>();

    if (!hitPanel) {

        return;

    }



    hitPanelBackground = hitPanel->GetBackgroundColor();

    hitPanelCaptured = true;

}



bool ButtonStyle::ShouldMutateVisuals() const

{

    return !isDestroying && !RTBEngine::Scene::SceneManager::GetInstance().IsSceneUnloading();

}



void ButtonStyle::RestoreHitPanelAppearance()

{

    if (!ShouldMutateVisuals()) {

        return;

    }



    if (!hitPanelCaptured || !hitPanel) {

        return;

    }



    const RTBEngine::Math::Vector4 current = hitPanel->GetBackgroundColor();

    if (current.x != hitPanelBackground.x ||

        current.y != hitPanelBackground.y ||

        current.z != hitPanelBackground.z ||

        current.w != hitPanelBackground.w) {

        hitPanel->SetBackgroundColor(hitPanelBackground);

    }

}



void ButtonStyle::RefreshBindings()

{

    RTBEngine::Scene::GameObject* go = GetOwner();

    if (!go) {

        return;

    }



    if (!label) {

        label = go->GetComponent<RTBEngine::UI::UIText>();

    }



    if (!defaultUIButtonVisualsDisabled) {

        if (RTBEngine::UI::UIButton* button = go->GetComponent<RTBEngine::UI::UIButton>()) {

            button->enableDefaultHoverVisuals = false;

            defaultUIButtonVisualsDisabled = true;

        }

    }



    if (backgroundImage) {

        warnedMissingImage = false;

    } else if (!warnedMissingImage) {

        RTB_WARN("ButtonStyle: assign backgroundImage for button visuals.");

        warnedMissingImage = true;

    }



    if (label) {

        warnedMissingLabel = false;

    } else if (!warnedMissingLabel) {

        RTB_WARN("ButtonStyle: missing label reference; text color animation is disabled.");

        warnedMissingLabel = true;

    }

    RestoreHitPanelAppearance();

}



bool ButtonStyle::IsButtonInteractable() const

{

    RTBEngine::Scene::GameObject* go = GetOwner();

    if (!go) {

        return true;

    }



    if (RTBEngine::UI::UIButton* button = go->GetComponent<RTBEngine::UI::UIButton>()) {

        return button->IsInteractable();

    }



    return true;

}



void ButtonStyle::SyncInteractionState()

{

    const bool interactable = IsButtonInteractable();

    if (interactable != lastInteractable) {

        lastInteractable = interactable;

        if (!interactable) {

            isPointerOver = false;

            isPressed = false;

            StartTransition(State::Disabled);

            RefreshUpdateTick();

            return;

        }



        StartTransition(ResolveStateFromInteraction());

        RefreshUpdateTick();

        return;

    }



    if (!interactable) {

        SetState(State::Disabled);

        ApplyTextureForState(currentState);



        RTBEngine::Math::Vector2 targetScale;

        float targetRotation = 0.0f;

        RTBEngine::Math::Vector4 targetTextColor;

        RTBEngine::Math::Vector4 targetImageTint;

        ResolveTargetVisuals(currentState, targetScale, targetRotation, targetTextColor, targetImageTint);

        FinishTransition(targetScale, targetRotation, targetTextColor, targetImageTint);

    }

    RefreshUpdateTick();

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

    if (!backgroundImage || baseTransformCaptured) {

        return;

    }



    baseScale = backgroundImage->GetScale();

    baseRotation = backgroundImage->GetRotation();

    currentScale = baseScale;

    currentRotation = baseRotation;

    baseTransformCaptured = true;

}



RTBEngine::Rendering::Texture* ButtonStyle::ResolveTextureForState(State state) const

{

    switch (state) {

    case State::Hover:

        if (hoverTexture) {

            return hoverTexture;

        }

        break;

    case State::Pressed:

        if (pressedTexture) {

            return pressedTexture;

        }

        break;

    case State::Disabled:

    case State::Normal:

    default:

        break;

    }



    if (normalTexture) {

        return normalTexture;

    }



    return backgroundImage ? backgroundImage->GetTexture() : nullptr;

}



void ButtonStyle::ApplyTextureForState(State state)

{

    if (!backgroundImage) {

        return;

    }



    if (RTBEngine::Rendering::Texture* texture = ResolveTextureForState(state)) {

        backgroundImage->SetTexture(texture);

    }

}



void ButtonStyle::ResolveTargetVisuals(State state,

                                       RTBEngine::Math::Vector2& outScale,

                                       float& outRotation,

                                       RTBEngine::Math::Vector4& outTextColor,

                                       RTBEngine::Math::Vector4& outImageTint) const

{

    switch (state)

    {

    case State::Hover:

        outScale = RTBEngine::Math::Vector2(baseScale.x * hoverScaleBoost, baseScale.y * hoverScaleBoost);

        outRotation = baseRotation + hoverRotationDeg;

        outTextColor  = hoverTextColor;

        outImageTint  = hoverImageTint;

        break;

    case State::Pressed:

        outScale = RTBEngine::Math::Vector2(baseScale.x * clickScaleBoost, baseScale.y * clickScaleBoost);

        outRotation = baseRotation;

        outTextColor  = clickTextColor;

        outImageTint  = clickImageTint;

        break;

    case State::Disabled:

        outScale = baseScale;

        outRotation = baseRotation;

        outTextColor  = disabledTextColor;

        outImageTint  = disabledImageTint;

        break;

    default:

        outScale = baseScale;

        outRotation = baseRotation;

        outTextColor  = normalTextColor;

        outImageTint  = normalImageTint;

        break;

    }

}



ButtonStyle::State ButtonStyle::ResolveStateFromInteraction() const

{

    if (!IsButtonInteractable()) {

        return State::Disabled;

    }



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



    if (toState == State::Disabled || fromState == State::Disabled) {

        return hoverOutTimeSec;

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



    if (toState == State::Normal || toState == State::Disabled) {

        return RTBEngine::Math::EaseInOutCubic(t);

    }



    return RTBEngine::Math::EaseOutCubic(t);

}



void ButtonStyle::ApplyCurrentVisuals()

{

    if (!ShouldMutateVisuals() || !backgroundImage) {

        return;

    }



    backgroundImage->SetTint(currentImageTint);

    backgroundImage->SetScale(currentScale);

    backgroundImage->SetRotation(currentRotation);



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

    (void)deltaTime;

    if (!ShouldMutateVisuals()) {

        return;

    }



    SyncInteractionState();
    RestoreHitPanelAppearance();
}



void ButtonStyle::RefreshUpdateTick()

{

    if (!ShouldMutateVisuals()) {

        return;

    }



    if (activeTransition.IsValid()) {

        SetUpdateTickEnabled(true);

        return;

    }



    if (!IsButtonInteractable()) {

        SetUpdateTickEnabled(true);

        return;

    }



    SetUpdateTickEnabled(false);

}



void ButtonStyle::OnDestroy()

{

    isDestroying = true;

    StopTransition();

    backgroundImage = nullptr;

    label = nullptr;

    hitPanel = nullptr;

    SetUpdateTickEnabled(false);

}



void ButtonStyle::OnPointerEnter(const RTBEngine::UI::PointerEventData&)

{

    if (!IsButtonInteractable()) {

        return;

    }



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

    if (!IsButtonInteractable()) {

        return;

    }



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

    if (!playClickSound || !IsButtonInteractable() || !clickAudio) {

        return;

    }

    clickAudio->PlayOneShot();

}



void ButtonStyle::StartTransition(State nextState)

{

    if (!ShouldMutateVisuals() || !backgroundImage) {

        return;

    }



    NormalizeTimingProperties();

    CaptureBaseVisualTransform();



    const RTBEngine::Math::Vector2 startScale = currentScale;

    const float startRotation = currentRotation;

    const RTBEngine::Math::Vector4 startTextColor = currentTextColor;

    const RTBEngine::Math::Vector4 startImageTint = currentImageTint;



    const State previousState = currentState;

    SetState(nextState);

    ApplyTextureForState(currentState);



    RTBEngine::Math::Vector2 targetScale;

    float targetRotation = 0.0f;

    RTBEngine::Math::Vector4 targetTextColor;

    RTBEngine::Math::Vector4 targetImageTint;

    ResolveTargetVisuals(currentState, targetScale, targetRotation, targetTextColor, targetImageTint);



    StopTransition();



    const float transitionDuration = ResolveTransitionDuration(previousState, currentState);

    if (transitionDuration <= 0.0f) {

        FinishTransition(targetScale, targetRotation, targetTextColor, targetImageTint);

        return;

    }



    const float safeAnimationTime = std::max(0.001f, transitionDuration);

    activeTransition = StartSequence(

        RTBEngine::Scripting::LatentSequence()

            .Tween(safeAnimationTime,

                [this, previousState, nextState, startScale, startRotation, startTextColor, startImageTint, targetScale, targetRotation, targetTextColor, targetImageTint](float t) {

                    const float easedT = EaseTransitionProgress(previousState, nextState, t);

                    currentScale = RTBEngine::Math::Lerp(startScale, targetScale, easedT);

                    currentRotation = RTBEngine::Math::Lerp(startRotation, targetRotation, easedT);

                    currentTextColor  = RTBEngine::Math::Lerp(startTextColor,  targetTextColor,  easedT);

                    currentImageTint  = RTBEngine::Math::Lerp(startImageTint,  targetImageTint,  easedT);

                    ApplyCurrentVisuals();

                },

                [this, targetScale, targetRotation, targetTextColor, targetImageTint]() {

                    FinishTransition(targetScale, targetRotation, targetTextColor, targetImageTint);

                    RefreshUpdateTick();

                })

    );



    if (!activeTransition.IsValid()) {

        FinishTransition(targetScale, targetRotation, targetTextColor, targetImageTint);

        return;

    }



    SetUpdateTickEnabled(true);

}



void ButtonStyle::StopTransition()

{

    if (activeTransition.IsValid()) {

        CancelInvoke(activeTransition);

        activeTransition = {};

    }

}



void ButtonStyle::FinishTransition(const RTBEngine::Math::Vector2& finalScale,

                                   float finalRotation,

                                   const RTBEngine::Math::Vector4& finalTextColor,

                                   const RTBEngine::Math::Vector4& finalImageTint)

{

    currentScale = finalScale;

    currentRotation = finalRotation;

    currentTextColor = finalTextColor;

    currentImageTint = finalImageTint;

    activeTransition = {};



    if (!ShouldMutateVisuals()) {

        return;

    }



    ApplyCurrentVisuals();

}


