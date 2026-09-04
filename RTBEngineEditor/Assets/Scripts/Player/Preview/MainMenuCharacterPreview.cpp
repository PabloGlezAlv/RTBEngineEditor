#include "MainMenuCharacterPreview.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>

using ThisClass = MainMenuCharacterPreview;

namespace {
    constexpr const char* kAnimIdle = "Idle";
}

RTB_REGISTER_COMPONENT(MainMenuCharacterPreview)
    RTB_PROPERTY(previewOffset)
    RTB_PROPERTY(previewYawDegrees)
    RTB_PROPERTY(previewScale)
RTB_END_REGISTER(MainMenuCharacterPreview)

void MainMenuCharacterPreview::OnDestroy()
{
    ClearPreviewInstance();
}

void MainMenuCharacterPreview::ClearPreviewInstance()
{
    if (!previewInstance) {
        currentCharacterId.clear();
        return;
    }

    RTBEngine::Scene::Scene* scene = RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
    if (scene) {
        scene->RemoveGameObject(previewInstance);
    }

    previewInstance = nullptr;
    currentCharacterId.clear();
}

void MainMenuCharacterPreview::ApplyPreviewTransform()
{
    if (!previewInstance) {
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

    auto* animator = previewInstance->GetComponentInChildren<RTBEngine::Animation::Animator>();
    if (!animator) {
        return;
    }

    if (animator->HasKey(kAnimIdle)) {
        animator->PlayKey(kAnimIdle);
        return;
    }

    animator->Stop();
}

void MainMenuCharacterPreview::ShowCharacterById(const std::string& characterId)
{
    if (characterId.empty()) {
        return;
    }

    if (characterId == currentCharacterId && previewInstance) {
        ApplyPreviewTransform();
        PlayIdleAnimation();
        return;
    }

    CharacterDefinition* definition = CharacterCatalog::GetInstance().GetById(characterId);
    if (!definition) {
        RTB_WARN("MainMenuCharacterPreview: Unknown character id '" + characterId + "'.");
        return;
    }

    if (definition->previewPrefabRef.empty()) {
        RTB_WARN("MainMenuCharacterPreview: No preview prefab for character '" + characterId + "'.");
        return;
    }

    RTBEngine::Core::ResourceManager& resources = RTBEngine::Core::ResourceManager::GetInstance();
    const std::string resolvedPath = resources.ResolvePathForRead(definition->previewPrefabRef);
    RTBEngine::Scene::Prefab* prefab =
        RTBEngine::Scene::PrefabRegistry::GetInstance().GetByPath(resolvedPath);
    if (!prefab) {
        RTB_WARN("MainMenuCharacterPreview: Preview prefab not found: '" + definition->previewPrefabRef + "'.");
        return;
    }

    ClearPreviewInstance();

    previewInstance = RTBEngine::Scene::SceneManager::GetInstance().Instantiate(*prefab, owner, true);
    if (!previewInstance) {
        RTB_WARN("MainMenuCharacterPreview: Failed to instantiate preview for '" + characterId + "'.");
        return;
    }

    currentCharacterId = characterId;
    previewInstance->SetActive(true);
    ApplyPreviewTransform();
    PlayIdleAnimation();
}
