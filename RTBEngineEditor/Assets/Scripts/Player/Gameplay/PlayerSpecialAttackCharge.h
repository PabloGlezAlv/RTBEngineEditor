#pragma once

#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>

namespace RTBEngine {
    namespace UI {
        class UIImage;
        class UIJoystick;
    }
}

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

    RTB_COMPONENT(PlayerSpecialAttackCharge)

    void OnStart() override;
    void OnValidate() override;
    void OnDestroy() override;

private:
    int currentHits = 0;
    bool loggedFullyCharged = false;

    bool IsLocalPlayer() const;
    void ClampSettings();
    void EnsureReferences();
    void ResolveImagesFromJoystick();
    void ApplyVisuals(bool forceReset);
    void ResetSceneJoystickVisuals();
    RTBEngine::UI::UIImage* GetBackgroundImage() const;
    RTBEngine::UI::UIImage* GetHandleImage() const;
};
