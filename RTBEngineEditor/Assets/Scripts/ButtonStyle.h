#pragma once
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/UI/EventSystem/IPointerEnterHandler.h>
#include <RTBEngine/UI/EventSystem/IPointerExitHandler.h>
#include <RTBEngine/UI/EventSystem/IPointerDownHandler.h>
#include <RTBEngine/UI/EventSystem/IPointerUpHandler.h>
#include <RTBEngine/UI/EventSystem/IPointerClickHandler.h>
#include <RTBEngine/UI/Elements/UIPanel.h>
#include <RTBEngine/UI/Elements/UIText.h>
#include <RTBEngine/Math/Math.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scripting/LatentActions.h>

namespace RTBEngine {
    namespace UI {
        class UIButton;
    }
}

class ButtonStyle : public RTBEngine::ECS::Component
                  , public RTBEngine::UI::IPointerEnterHandler
                  , public RTBEngine::UI::IPointerExitHandler
                  , public RTBEngine::UI::IPointerDownHandler
                  , public RTBEngine::UI::IPointerUpHandler
                  , public RTBEngine::UI::IPointerClickHandler
{
public:
    ButtonStyle() = default;
    virtual ~ButtonStyle() = default;

    // Inspector-exposed references
    RTBEngine::UI::UIPanel* backgroundPanel = nullptr;
    RTBEngine::UI::UIText*  label           = nullptr;

    // Normal state
    RTBEngine::Math::Vector4 normalPanelColor = RTBEngine::Math::Vector4(0.2f, 0.2f, 0.2f, 1.0f);
    RTBEngine::Math::Vector4 normalTextColor  = RTBEngine::Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    // Hover state
    RTBEngine::Math::Vector4 hoverPanelColor  = RTBEngine::Math::Vector4(0.35f, 0.35f, 0.65f, 1.0f);
    RTBEngine::Math::Vector4 hoverTextColor   = RTBEngine::Math::Vector4(1.0f, 1.0f, 0.6f, 1.0f);
    float hoverScaleBoost  = 1.06f;
    float hoverRotationDeg = 0.0f;

    // Click / press state
    RTBEngine::Math::Vector4 clickPanelColor  = RTBEngine::Math::Vector4(0.15f, 0.15f, 0.45f, 1.0f);
    RTBEngine::Math::Vector4 clickTextColor   = RTBEngine::Math::Vector4(0.86f, 0.86f, 0.92f, 1.0f);
    float clickScaleBoost = 0.96f;

    // Animation
    float hoverInTimeSec  = 0.12f;
    float hoverOutTimeSec = 0.18f;
    float pressInTimeSec  = 0.08f;
    float pressOutTimeSec = 0.12f;

    RTB_COMPONENT(ButtonStyle)

    virtual void OnAwake() override;
    virtual void OnStart() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnDestroy() override;
    virtual void OnValidate() override;

    virtual void OnPointerEnter(const RTBEngine::UI::PointerEventData&) override;
    virtual void OnPointerExit(const RTBEngine::UI::PointerEventData&) override;
    virtual void OnPointerDown(const RTBEngine::UI::PointerEventData&) override;
    virtual void OnPointerUp(const RTBEngine::UI::PointerEventData&) override;
    virtual void OnPointerClick(const RTBEngine::UI::PointerEventData&) override;

private:
    enum class State { Normal, Hover, Pressed };
    State currentState = State::Normal;

    RTBEngine::Math::Vector2 baseScale    = RTBEngine::Math::Vector2(1.0f, 1.0f);
    float                    baseRotation = 0.0f;
    float                    baseLabelVisualScale = 1.0f;

    RTBEngine::Math::Vector2 currentScale;
    float                    currentRotation = 0.0f;
    RTBEngine::Math::Vector4 currentPanelColor;
    RTBEngine::Math::Vector4 currentTextColor;
    RTBEngine::Scripting::LatentActionRunner latentRunner;
    RTBEngine::Scripting::LatentActionHandle activeTransition;
    bool                     baseTransformCaptured = false;
    bool                     defaultUIButtonVisualsDisabled = false;
    bool                     warnedMissingPanel = false;
    bool                     isPointerOver = false;
    bool                     isPressed = false;

    void RefreshBindings();
    void CaptureBaseVisualTransform();
    void NormalizeTimingProperties();
    void ResolveTargetVisuals(State state,
                              RTBEngine::Math::Vector2& outScale,
                              float& outRotation,
                              RTBEngine::Math::Vector4& outPanelColor,
                              RTBEngine::Math::Vector4& outTextColor) const;
    State ResolveStateFromInteraction() const;
    float ResolveTransitionDuration(State fromState, State toState) const;
    float EaseTransitionProgress(State fromState, State toState, float t) const;
    void ApplyCurrentVisuals();
    void StartTransition(State nextState);
    void StopTransition();
    void FinishTransition(const RTBEngine::Math::Vector2& finalScale,
                          float finalRotation,
                          const RTBEngine::Math::Vector4& finalPanelColor,
                          const RTBEngine::Math::Vector4& finalTextColor);
    void SetState(State nextState);
};
