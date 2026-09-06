#pragma once

#include "PlayerAimTrailPresenter.h"

#include <RTBEngine/Core/Event.h>
#include <RTBEngine/Math/Vectors/Vector2.h>
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

#include <cstdint>

namespace RTBEngine {
    namespace Scene {
        class GameObject;
        class TrailRenderer;
    }

    namespace UI {
        class UIJoystick;
    }
}

class CharacterAbility;
class PlayerAmmoSystem;
class ThirdPersonCharacterController;

class PlayerBasicAttackDriver : public RTBEngine::Scene::Component
{
public:
    PlayerBasicAttackDriver() = default;
    ~PlayerBasicAttackDriver() override = default;

    void OnStart() override;
    void OnValidate() override;
    void OnDestroy() override;

    void BindController(ThirdPersonCharacterController* controller);
    void RefreshBindings();

    bool IsAiming() const { return combatPhase == CombatPhase::Aiming; }
    bool IsAttacking() const { return combatPhase == CombatPhase::Attacking; }
    bool IsBusy() const { return combatPhase != CombatPhase::Idle; }

    void UpdateLocalPredictedVisual(float deltaTime);
    void UpdateLocalAimInput(float deltaTime);
    void UpdateFixedAiming(float deltaTime);
    void UpdateFixedAttacking(float deltaTime);
    void CancelForMenu();
    void CancelForDeath();
    void HideAimVisuals();
    void ResetAfterRevive();
    void TryProcessRemoteAttack();
    void PlayReplicatedVisual(const RTBEngine::Math::Vector3& attackDirection);

    RTBEngine::Math::Vector3 GetPlanarAttackDirectionFromJoystick(
        const RTBEngine::Math::Vector2& joystickValue) const;
    void ApplyCombatAnimationOverrides(
        const std::string& aimDrawFbx,
        const std::string& aimLoopFbx,
        const std::string& attackFbx);

    RTBEngine::UI::UIJoystick* GetAttackJoystick() const { return attackJoystick; }
    void SetAttackJoystick(RTBEngine::UI::UIJoystick* joystick);

    RTB_COMPONENT(PlayerBasicAttackDriver)

    RTB_SERIALIZE()
    CharacterAbility* attackAbility = nullptr;
    RTB_SERIALIZE()
    RTBEngine::UI::UIJoystick* attackJoystick = nullptr;
    RTB_SERIALIZE()
    RTBEngine::Scene::TrailRenderer* attackAimTrail = nullptr;
    RTB_SERIALIZE()
    float aimTrailForwardOffset = 0.40f;
    RTB_SERIALIZE()
    float aimTrailHeightOffset = 0.0f;
    RTB_SERIALIZE()
    float aimTrailWallClipRadius = 0.45f;
    RTB_SERIALIZE()
    RTBEngine::Scene::GameObject* aimArrowVisual = nullptr;

private:
    enum class CombatPhase {
        Idle,
        Aiming,
        Attacking
    };

    enum class AimPhase {
        Draw,
        Hold
    };

    void RebindAttackJoystickSubscription();
    void UnsubscribeFromAttackJoystick();
    void RebindAnimatorKeySubscriptions();
    void UnsubscribeFromAnimatorKeys();
    void HandleJoystickAttackReleased(const RTBEngine::Math::Vector2& joystickValue);
    void HandleAttackReleasedWithDirection(const RTBEngine::Math::Vector3& attackDirection);
    bool CanStartAiming() const;
    void TryBeginAiming();
    void UpdateAimingState(float deltaTime);
    void FinishAiming();
    void SetAimArrowVisible(bool visible);
    void UpdateAttackAimTrail();
    void HideAttackAimTrail();
    void StartAttack(const RTBEngine::Math::Vector3& attackDirection);
    void FinishAttack();
    void PlayPredictedAttackVisual(const RTBEngine::Math::Vector3& attackDirection);
    void PollAttackCompletion(float deltaTime);
    void ScheduleAttackSafetyTimeout();
    void ClearAttackSafetyTimeout();
    float GetAimRangeForVisual() const;
    RTBEngine::Math::Vector3 ResolveAimDirection() const;
    RTBEngine::Math::Vector3 GetAttackDirectionFromJoystick(
        const RTBEngine::Math::Vector2& joystickValue) const;
    RTBEngine::Math::Vector3 GetActiveAttackDirection() const;

    ThirdPersonCharacterController* controller = nullptr;
    PlayerAmmoSystem* ammoSystem = nullptr;
    CombatPhase combatPhase = CombatPhase::Idle;
    AimPhase aimPhase = AimPhase::Draw;
    bool wasDraggingJoystick = false;
    RTBEngine::Math::Vector2 cachedAttackJoystickValue = RTBEngine::Math::Vector2::Zero();
    float attackStateElapsed = 0.0f;
    bool attackAbilitySafetyExpired = false;
    RTBEngine::Scripting::LatentActionHandle attackSafetyHandle;
    RTBEngine::UI::UIJoystick* subscribedAttackJoystick = nullptr;
    RTBEngine::Core::EventSubscription attackJoystickReleaseSubscription;
    RTBEngine::Core::EventSubscription aimDrawFinishedSubscription;
    RTBEngine::Core::EventSubscription attackFinishedSubscription;
    bool attackClipFinished = false;
    std::uint32_t lastProcessedRemoteAttackSequence = 0;
    RTBEngine::Math::Vector3 activeAttackDirection = RTBEngine::Math::Vector3::Zero();
    PlayerAimTrailPresenter aimTrailPresenter;
};
