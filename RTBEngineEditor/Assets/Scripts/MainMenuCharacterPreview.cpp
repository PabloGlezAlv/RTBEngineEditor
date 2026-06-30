#include "MainMenuCharacterPreview.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"
#include "PlayerCharacterSelection.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>

using ThisClass = MainMenuCharacterPreview;

namespace {
    constexpr const char* kPreviewIdleAlias = "MenuPreviewIdle";
}

RTB_REGISTER_COMPONENT(MainMenuCharacterPreview)
    RTB_PROPERTY(previewOffset)
    RTB_PROPERTY(previewYawDegrees)
    RTB_PROPERTY(previewScale)
    RTB_PROPERTY_FBX(idleAnimationFbx)
RTB_END_REGISTER(MainMenuCharacterPreview)

void MainMenuCharacterPreview::OnStart()
{
    const std::string selectedId = PlayerCharacterSelection::GetInstance().GetSelectedCharacterId();
    ShowCharacterById(selectedId.empty() ? "ranger" : selectedId);
}

void MainMenuCharacterPreview::ResolvePreviewInstance()
{
    if (previewInstance || !owner) {
        return;
    }

    for (RTBEngine::ECS::GameObject* child : owner->GetChildren()) {
        if (!child) {
            continue;
        }

        if (child->GetComponent<RTBEngine::Animation::Animator>()) {
            previewInstance = child;
            return;
        }
    }

    RTB_WARN("MainMenuCharacterPreview: No character preview prefab found under CharacterPreviewStage.");
}

void MainMenuCharacterPreview::ApplyPreviewTransform()
{
    if (!previewInstance || !owner) {
        return;
    }

    previewInstance->GetTransform().SetPosition(previewOffset);
    previewInstance->GetTransform().SetRotation(
        RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, previewYawDegrees, 0.0f));
    previewInstance->GetTransform().SetScale(
        RTBEngine::Math::Vector3(previewScale, previewScale, previewScale));
}

void MainMenuCharacterPreview::PlayIdleAnimation()
{
    if (!previewInstance) {
        return;
    }

    auto* animator = previewInstance->GetComponent<RTBEngine::Animation::Animator>();
    if (!animator) {
        return;
    }

    bool clipReady = false;
    if (!idleAnimationFbx.empty()) {
        // Load the clip only once; PlayIdleAnimation may run again (e.g. on character
        // switches) and re-loading would overwrite the clip and log a collision warning.
        clipReady = animator->GetClip(kPreviewIdleAlias) != nullptr
            || animator->LoadClipFromFbx(kPreviewIdleAlias, idleAnimationFbx);
        if (clipReady) {
            animator->Play(kPreviewIdleAlias, true);
        }
    }

    if (!clipReady) {
        animator->Stop();
    }
}

void MainMenuCharacterPreview::ShowCharacterById(const std::string& characterId)
{
    if (characterId.empty()) {
        return;
    }

    CharacterDefinition* definition = CharacterCatalog::GetInstance().GetById(characterId);
    if (!definition) {
        RTB_WARN("MainMenuCharacterPreview: Unknown character id '" + characterId + "'.");
        return;
    }

    ResolvePreviewInstance();
    if (!previewInstance) {
        return;
    }

    previewInstance->SetActive(true);
    ApplyPreviewTransform();
    PlayIdleAnimation();
}
