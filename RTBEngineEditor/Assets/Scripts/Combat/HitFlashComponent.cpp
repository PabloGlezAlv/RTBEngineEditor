#include "HitFlashComponent.h"

#include <RTBEngine/Rendering/Material.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/MeshRenderer.h>

#include <algorithm>
#include <functional>

using ThisClass = HitFlashComponent;

RTB_REGISTER_COMPONENT(HitFlashComponent)
    RTB_PROPERTY_COMPONENT(health, HealthComponent)
    RTB_PROPERTY_COLOR(flashColor)
    RTB_PROPERTY_RANGE(flashDuration, 0.0f, 1.0f)
    RTB_PROPERTY(includeChildren)
RTB_END_REGISTER(HitFlashComponent)

void HitFlashComponent::OnStart()
{
    if (owner) {
        health = owner->GetComponent<HealthComponent>();
    }

    CollectFlashTargets();
    RebindDamageSubscription();
}

void HitFlashComponent::OnValidate()
{
    flashDuration = std::max(0.0f, flashDuration);
    CollectFlashTargets();
}

void HitFlashComponent::TriggerFlash()
{
    BeginFlash();
}

void HitFlashComponent::OnLateUpdate(float deltaTime)
{
    if (flashTimeRemaining <= 0.0f || flashDuration <= 0.0f) {
        return;
    }

    flashTimeRemaining = std::max(0.0f, flashTimeRemaining - std::max(0.0f, deltaTime));
    const float strength = flashTimeRemaining / flashDuration;
    ApplyFlashStrength(strength);

    if (flashTimeRemaining <= 0.0f) {
        RestoreBaseColors();
    }
}

void HitFlashComponent::OnDestroy()
{
    UnbindDamageSubscription();
    flashTargets.clear();
    flashTimeRemaining = 0.0f;
}

void HitFlashComponent::RebindDamageSubscription()
{
    UnbindDamageSubscription();

    if (!health) {
        return;
    }

    damageSubscription = health->SubscribeToDamageTaken(
        [this](const HealthComponent::DamageTakenEvent& eventData) {
            HandleDamageTaken(eventData);
        });
}

void HitFlashComponent::UnbindDamageSubscription()
{
    damageSubscription.Reset();
}

void HitFlashComponent::CollectFlashTargets()
{
    flashTargets.clear();
    const std::function<void(RTBEngine::Scene::GameObject*)> collectFromObject =
        [&](RTBEngine::Scene::GameObject* object) {
            if (!object) {
                return;
            }

            if (auto* renderer = object->GetComponent<RTBEngine::Scene::MeshRenderer>()) {
                FlashTarget target;
                target.renderer = renderer;
                target.baseColor = renderer->colorRef;
                if (RTBEngine::Rendering::Material* material = renderer->GetMaterial()) {
                    target.baseDiffuseColor = material->GetDiffuseColor();
                }
                flashTargets.push_back(target);
            }

            if (!includeChildren) {
                return;
            }

            for (RTBEngine::Scene::GameObject* child : object->GetChildren()) {
                collectFromObject(child);
            }
        };

    collectFromObject(owner);
}

void HitFlashComponent::HandleDamageTaken(const HealthComponent::DamageTakenEvent& eventData)
{
    if (eventData.damage.amount <= 0.0f || flashDuration <= 0.0f) {
        return;
    }

    if (flashTimeRemaining > 0.0f) {
        flashTimeRemaining = flashDuration;
        return;
    }

    BeginFlash();
}

void HitFlashComponent::BeginFlash()
{
    if (flashDuration <= 0.0f) {
        return;
    }

    if (flashTargets.empty()) {
        CollectFlashTargets();
    }

    if (flashTargets.empty()) {
        return;
    }

    flashTimeRemaining = flashDuration;
    ApplyFlashStrength(1.0f);
}

void HitFlashComponent::ApplyFlashStrength(float strength)
{
    const float clampedStrength = std::clamp(strength, 0.0f, 1.0f);

    for (FlashTarget& target : flashTargets) {
        if (!target.renderer) {
            continue;
        }

        const RTBEngine::Math::Vector4 lerpedColor = RTBEngine::Math::Vector4(
            RTBEngine::Math::Lerp(target.baseColor.x, flashColor.x, clampedStrength),
            RTBEngine::Math::Lerp(target.baseColor.y, flashColor.y, clampedStrength),
            RTBEngine::Math::Lerp(target.baseColor.z, flashColor.z, clampedStrength),
            RTBEngine::Math::Lerp(target.baseColor.w, flashColor.w, clampedStrength));

        PushColorToRenderer(target, lerpedColor);
    }
}

void HitFlashComponent::PushColorToRenderer(FlashTarget& target,
                                            const RTBEngine::Math::Vector4& color)
{
    if (!target.renderer) {
        return;
    }

    target.renderer->colorRef = color;

    const auto applyToMaterial = [&](RTBEngine::Rendering::Material* material) {
        if (!material) {
            return;
        }

        material->SetColor(color);
    };

    applyToMaterial(target.renderer->GetMaterial());

    if (target.renderer->IsMultiMesh()) {
        for (int meshIndex = 0; meshIndex < target.renderer->GetMeshCount(); ++meshIndex) {
            applyToMaterial(target.renderer->GetMaterialForMesh(meshIndex));
        }
    }
}

void HitFlashComponent::RestoreBaseColors()
{
    for (FlashTarget& target : flashTargets) {
        if (!target.renderer) {
            continue;
        }

        PushColorToRenderer(target, target.baseColor);

        const auto restoreDiffuse = [&](RTBEngine::Rendering::Material* material) {
            if (!material) {
                return;
            }

            material->SetDiffuseColor(target.baseDiffuseColor);
        };

        restoreDiffuse(target.renderer->GetMaterial());

        if (target.renderer->IsMultiMesh()) {
            for (int meshIndex = 0; meshIndex < target.renderer->GetMeshCount(); ++meshIndex) {
                restoreDiffuse(target.renderer->GetMaterialForMesh(meshIndex));
            }
        }
    }
}
