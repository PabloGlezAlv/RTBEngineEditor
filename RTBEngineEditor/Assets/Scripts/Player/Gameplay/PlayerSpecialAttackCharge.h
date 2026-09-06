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

class IPlayerSpecialAttack;
class ThirdPersonCharacterController;

class PlayerSpecialAttackCharge : public RTBEngine::Scene::Component
{
public:
    PlayerSpecialAttackCharge() = default;
    ~PlayerSpecialAttackCharge() override;

    void SetSpecialAttackJoystick(RTBEngine::UI::UIJoystick* joystick);
    void SetReadyIcon(RTBEngine::UI::UIImage* icon);
    RTBEngine::UI::UIJoystick* GetSpecialAttackJoystick() const { return specialAttackJoystick; }
    RTBEngine::UI::UIImage* GetReadyIcon() const { return readyIcon; }

    void RegisterSuccessfulHit();
    bool IsReady() const;
    float GetChargeNormalized() const;
    void RefreshAfterSpawn();
    bool TryGetSpecialAimDirection(RTBEngine::Math::Vector3& outAimDirection) const;

    void OnStart() override;
    void OnValidate() override;
    void OnLateUpdate(float deltaTime) override;
    void OnDestroy() override;

    RTB_COMPONENT(PlayerSpecialAttackCharge)

    RTB_SERIALIZE()
    RTBEngine::UI::UIJoystick* specialAttackJoystick = nullptr;
    RTB_SERIALIZE()
    RTBEngine::UI::UIImage* readyIcon = nullptr;
    RTB_SERIALIZE()
    int hitsToFullyCharge = 5;

    int currentHits = 0;
    RTBEngine::UI::UIJoystick* subscribedSpecialJoystick = nullptr;
    RTBEngine::Core::EventSubscription specialJoystickReleaseSubscription;
    IPlayerSpecialAttack* specialAttack = nullptr;
    ThirdPersonCharacterController* controller = nullptr;

    bool IsLocalPlayer() const;
    void ClampSettings();
    void CacheGameplayReferences();
    void ApplyVisuals(bool forceReset);
    void ResetSceneJoystickVisuals();
    void RebindSpecialJoystickSubscription();
    void UnsubscribeFromSpecialJoystick();
    void HandleSpecialJoystickReleased(const RTBEngine::Math::Vector2& joystickValue);
    bool ConsumeCharge();
    RTBEngine::UI::UIImage* GetBackgroundImage() const;
    RTBEngine::UI::UIImage* GetHandleImage() const;
};
