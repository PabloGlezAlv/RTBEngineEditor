#pragma once

#include <RTBEngine/Scene/Component.h>

#include <RTBEngine/UI/EventSystem/IPointerEnterHandler.h>

#include <RTBEngine/UI/EventSystem/IPointerExitHandler.h>

#include <RTBEngine/UI/EventSystem/IPointerDownHandler.h>

#include <RTBEngine/UI/EventSystem/IPointerUpHandler.h>

#include <RTBEngine/UI/EventSystem/IPointerClickHandler.h>

#include <RTBEngine/UI/Elements/UIText.h>

#include <RTBEngine/UI/UIElement.h>

#include <RTBEngine/Math/Math.h>

#include <RTBEngine/Reflection/PropertyMacros.h>



namespace RTBEngine {

    namespace Scene {
        class AudioSourceComponent;
    }

    namespace Rendering {

        class Texture;

    }

    namespace UI {

        class UIButton;

        class UIImage;

        class UIPanel;

    }

}



class ButtonStyle : public RTBEngine::Scene::Component

                  , public RTBEngine::UI::IPointerEnterHandler

                  , public RTBEngine::UI::IPointerExitHandler

                  , public RTBEngine::UI::IPointerDownHandler

                  , public RTBEngine::UI::IPointerUpHandler

                  , public RTBEngine::UI::IPointerClickHandler

{

public:

    ButtonStyle() = default;

    virtual ~ButtonStyle() = default;



    RTBEngine::UI::UIImage* backgroundImage = nullptr;

    RTBEngine::UI::UIText* label = nullptr;



    RTBEngine::Rendering::Texture* normalTexture = nullptr;

    RTBEngine::Rendering::Texture* hoverTexture = nullptr;

    RTBEngine::Rendering::Texture* pressedTexture = nullptr;



    RTBEngine::Math::Vector4 normalTextColor  = RTBEngine::Math::Vector4(0.96f, 0.94f, 0.88f, 1.0f);

    RTBEngine::Math::Vector4 normalImageTint  = RTBEngine::Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);



    RTBEngine::Math::Vector4 hoverTextColor   = RTBEngine::Math::Vector4(1.0f, 0.82f, 0.47f, 1.0f);

    RTBEngine::Math::Vector4 hoverImageTint   = RTBEngine::Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    float hoverScaleBoost  = 1.05f;

    float hoverRotationDeg = 0.0f;



    RTBEngine::Math::Vector4 clickTextColor   = RTBEngine::Math::Vector4(0.86f, 0.84f, 0.78f, 1.0f);

    RTBEngine::Math::Vector4 clickImageTint   = RTBEngine::Math::Vector4(0.92f, 0.92f, 0.92f, 1.0f);

    float clickScaleBoost = 0.97f;



    RTBEngine::Math::Vector4 disabledTextColor  = RTBEngine::Math::Vector4(0.55f, 0.54f, 0.50f, 1.0f);

    RTBEngine::Math::Vector4 disabledImageTint  = RTBEngine::Math::Vector4(0.45f, 0.45f, 0.45f, 1.0f);



    float hoverInTimeSec  = 0.12f;

    float hoverOutTimeSec = 0.18f;

    float pressInTimeSec  = 0.08f;

    float pressOutTimeSec = 0.12f;

    bool playClickSound = true;
    RTBEngine::Scene::AudioSourceComponent* clickAudio = nullptr;

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

    enum class State { Normal, Hover, Pressed, Disabled };

    State currentState = State::Normal;



    RTBEngine::Math::Vector2 baseScale = RTBEngine::Math::Vector2(1.0f, 1.0f);

    float                    baseRotation = 0.0f;

    RTBEngine::Math::Vector2 currentScale = RTBEngine::Math::Vector2(1.0f, 1.0f);

    float                    currentRotation = 0.0f;

    RTBEngine::Math::Vector4 currentTextColor;

    RTBEngine::Math::Vector4 currentImageTint;

    RTBEngine::Scripting::LatentActionHandle activeTransition;

    bool                     baseTransformCaptured = false;

    bool                     defaultUIButtonVisualsDisabled = false;

    bool                     warnedMissingImage = false;

    bool                     warnedMissingLabel = false;

    bool                     isPointerOver = false;

    bool                     isPressed = false;

    bool                     lastInteractable = true;

    RTBEngine::UI::UIPanel*  hitPanel = nullptr;
    RTBEngine::Math::Vector4 hitPanelBackground = RTBEngine::Math::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    bool                     hitPanelCaptured = false;
    bool                     isDestroying = false;

    bool ShouldMutateVisuals() const;

    void CaptureHitPanelAppearance();
    void RestoreHitPanelAppearance();
    void RefreshBindings();

    void CaptureBaseVisualTransform();

    void NormalizeTimingProperties();

    void InitializeVisualState(bool enableRuntimePolling);

    bool IsButtonInteractable() const;

    RTBEngine::Rendering::Texture* ResolveTextureForState(State state) const;

    void ApplyTextureForState(State state);

    void ResolveTargetVisuals(State state,

                              RTBEngine::Math::Vector2& outScale,

                              float& outRotation,

                              RTBEngine::Math::Vector4& outTextColor,

                              RTBEngine::Math::Vector4& outImageTint) const;

    State ResolveStateFromInteraction() const;

    float ResolveTransitionDuration(State fromState, State toState) const;

    float EaseTransitionProgress(State fromState, State toState, float t) const;

    void ApplyCurrentVisuals();

    void StartTransition(State nextState);

    void StopTransition();

    void RefreshUpdateTick();

    void FinishTransition(const RTBEngine::Math::Vector2& finalScale,

                          float finalRotation,

                          const RTBEngine::Math::Vector4& finalTextColor,

                          const RTBEngine::Math::Vector4& finalImageTint);

    void SetState(State nextState);

    void SyncInteractionState();

};


