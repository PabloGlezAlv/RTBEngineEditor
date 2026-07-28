#pragma once

#include "ICharacterStatReceiver.h"

#include <RTBEngine/Math/Vectors/Vector4.h>
#include <RTBEngine/Reflection/PropertyMacros.h>
#include <RTBEngine/Scene/Component.h>
#include <RTBEngine/UI/Elements/UIPanel.h>

namespace RTBEngine {
    namespace UI {
        class UISlider;
    }
}

class CharacterDefinition;

class PlayerAmmoSystem : public RTBEngine::Scene::Component, public ICharacterStatReceiver
{
public:
    PlayerAmmoSystem() = default;
    ~PlayerAmmoSystem() override = default;

    bool CanFire() const;
    void ConsumeShot();
    void RefillAmmo();
    void RefreshNetworkState();
    float GetNormalizedAmmo() const { return normalizedAmmo; }

    static bool HasAmmoAvailable(RTBEngine::Scene::GameObject* instigator);
    static bool TryConsumeAttackAmmo(RTBEngine::Scene::GameObject* instigator);

    void ApplyCharacterStats(const CharacterDefinition& definition) override;

    void OnStart() override;
    void OnLateUpdate(float deltaTime) override;
    void OnValidate() override;

    RTB_COMPONENT(PlayerAmmoSystem)

    RTB_SERIALIZE()
    RTBEngine::UI::UISlider* ammoSlider = nullptr;
    RTB_SERIALIZE()
    RTBEngine::UI::UIPanel* ammoFillPanel = nullptr;
    RTB_SERIALIZE()
    int maxShots = 3;
    RTB_SERIALIZE()
    float fullReloadDuration = 10.0f;
    RTB_SERIALIZE()
    RTBEngine::Math::Vector4 fullAmmoColor = RTBEngine::Math::Vector4(0.18f, 0.52f, 0.95f, 1.0f);
    RTB_SERIALIZE()
    RTBEngine::Math::Vector4 partialAmmoColor = RTBEngine::Math::Vector4(0.52f, 0.72f, 0.92f, 0.55f);

    float normalizedAmmo = 1.0f;

    bool IsLocalPlayer() const;
    float GetEffectiveReloadDuration() const;
    void EnsureReferences();
    void SetBarVisible(bool visible);
    void UpdateVisuals();
    void RechargeAmmo(float deltaTime);
    RTBEngine::Math::Vector4 EvaluateFillColor() const;
    float GetShotCost() const;
};
