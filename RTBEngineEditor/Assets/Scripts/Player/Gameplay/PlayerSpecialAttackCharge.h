#pragma once

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace UI {
        class UIImage;
        class UIJoystick;
    }
}

class PlayerSpecialBeamAttack;
class ThirdPersonCharacterController;

class PlayerSpecialAttackCharge : public RTBEngine::Scene::Component
{
public:
    PlayerSpecialAttackCharge() = default;
    ~PlayerSpecialAttackCharge() override;

    RTBEngine::UI::UIJoystick* specialAttackJoystick = nullptr;
    RTBEngine::UI::UIImage* readyIcon = nullptr;
    int hitsToFullyCharge = 5;

    void RegisterSuccessfulHit();
    bool IsReady() const;
    float GetChargeNormalized() const;
    void RefreshAfterSpawn();
    bool TryGetSpecialAimDirection(RTBEngine::Math::Vector3& outAimDirection) const;

    RTB_COMPONENT(PlayerSpecialAttackCharge)

    void OnStart() override;
    void OnValidate() override;
    void OnLateUpdate(float deltaTime) override;
    void OnDestroy() override;

private:
    int currentHits = 0;
    RTBEngine::UI::UIJoystick* subscribedSpecialJoystick = nullptr;
    RTBEngine::Core::EventSubscription specialJoystickReleaseSubscription;
    PlayerSpecialBeamAttack* beamAttack = nullptr;
    ThirdPersonCharacterController* controller = nullptr;

    bool IsLocalPlayer() const;
    void ClampSettings();
    void CacheGameplayReferences();
    void ValidateRequiredReferences() const;
    void ApplyVisuals(bool forceReset);
    void ResetSceneJoystickVisuals();
    void RebindSpecialJoystickSubscription();
    void UnsubscribeFromSpecialJoystick();
    void HandleSpecialJoystickReleased(const RTBEngine::Math::Vector2& joystickValue);
    bool ConsumeCharge();
    RTBEngine::UI::UIImage* GetBackgroundImage() const;
    RTBEngine::UI::UIImage* GetHandleImage() const;
};
