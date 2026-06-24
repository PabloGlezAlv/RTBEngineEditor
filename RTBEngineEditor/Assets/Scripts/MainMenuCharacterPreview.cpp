#include "MainMenuCharacterPreview.h"

#include "CharacterCatalog.h"
#include "CharacterDefinition.h"
#include "PlayerCharacterSelection.h"

#include <RTBEngine/Animation/Animator.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Rendering/FbxBinding.h>
#include <RTBEngine/Rendering/ModelLoader.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>

using ThisClass = MainMenuCharacterPreview;

namespace {
    constexpr const char* kPreviewIdleAlias = "MainMenuPreview.Idle";
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
    if (!selectedId.empty()) {
        ShowCharacterById(selectedId);
    }
}

void MainMenuCharacterPreview::OnDestroy()
{
    ClearPreview();
}

void MainMenuCharacterPreview::ClearPreview()
{
    if (!spawnedPreview) {
        loadedModelRef.clear();
        return;
    }

    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (scene) {
        scene->RemoveGameObject(spawnedPreview);
    }

    spawnedPreview = nullptr;
    loadedModelRef.clear();
}

void MainMenuCharacterPreview::ApplyPreviewTransform()
{
    if (!spawnedPreview || !owner) {
        return;
    }

    spawnedPreview->SetParent(owner);
    spawnedPreview->GetTransform().SetPosition(previewOffset);
    spawnedPreview->GetTransform().SetRotation(
        RTBEngine::Math::Quaternion::FromEulerAngles(0.0f, previewYawDegrees, 0.0f));
    spawnedPreview->GetTransform().SetScale(
        RTBEngine::Math::Vector3(previewScale, previewScale, previewScale));
}

void MainMenuCharacterPreview::PlayIdleAnimation()
{
    if (!spawnedPreview) {
        return;
    }

    auto* animator = spawnedPreview->GetComponent<RTBEngine::Animation::Animator>();
    if (!animator) {
        return;
    }

    if (!idleAnimationFbx.empty()) {
        animator->LoadClipFromFbx(kPreviewIdleAlias, idleAnimationFbx);
    }

    if (animator->GetClip(kPreviewIdleAlias)) {
        animator->Play(kPreviewIdleAlias, true);
        return;
    }

    const auto clipNames = animator->GetClipNames();
    for (const std::string& clipName : clipNames) {
        if (clipName.find("Idle") != std::string::npos) {
            animator->Play(clipName, true);
            return;
        }
    }

    if (!clipNames.empty()) {
        animator->Play(clipNames.front(), true);
    }
}

void MainMenuCharacterPreview::ShowDefinition(const CharacterDefinition& definition)
{
    if (definition.modelRef.empty()) {
        ClearPreview();
        return;
    }

    if (spawnedPreview && loadedModelRef == definition.modelRef) {
        ApplyPreviewTransform();
        PlayIdleAnimation();
        return;
    }

    ClearPreview();

    RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
    if (!scene || !owner) {
        RTB_WARN("MainMenuCharacterPreview: No active scene to spawn preview.");
        return;
    }

    auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
    const RTBEngine::Rendering::ModelData modelData =
        RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(definition.modelRef);
    if (modelData.meshes.empty()) {
        RTB_WARN("MainMenuCharacterPreview: Model has no meshes: " + definition.modelRef);
        return;
    }

    spawnedPreview = RTBEngine::Rendering::BuildFbxHierarchy(
        scene, modelData, definition.modelRef, resources);
    if (!spawnedPreview) {
        return;
    }

    loadedModelRef = definition.modelRef;
    scene->BringGameObjectToLife(spawnedPreview);
    ApplyPreviewTransform();
    PlayIdleAnimation();
}

void MainMenuCharacterPreview::ShowCharacterById(const std::string& characterId)
{
    if (characterId.empty()) {
        ClearPreview();
        return;
    }

    CharacterDefinition* definition = CharacterCatalog::GetInstance().GetById(characterId);
    if (!definition) {
        ClearPreview();
        return;
    }

    ShowDefinition(*definition);
}
