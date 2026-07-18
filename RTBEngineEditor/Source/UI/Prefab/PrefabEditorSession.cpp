#include "PrefabEditorSession.h"
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/LightComponent.h>
#include <RTBEngine/Rendering/Lighting/Light.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <RTBEngine/Scripting/PrefabSaver.h>
#include <RTBEngine/Core/Logger.h>
#include <filesystem>
#include <vector>

namespace RTBEditor {

    bool PrefabEditorSession::IsEditorUtilityObject(const RTBEngine::Scene::GameObject* gameObject)
    {
        return gameObject && gameObject->GetName() == EditorUtilityLightName;
    }

    void PrefabEditorSession::SetupEditorLighting()
    {
        if (!stagingScene) {
            return;
        }

        auto* lightObject = new RTBEngine::Scene::GameObject(EditorUtilityLightName);
        lightObject->GetTransform().SetRotation(
            RTBEngine::Math::Quaternion::FromEulerAngles(50.0f, -30.0f, 0.0f));

        auto* lightComponent = new RTBEngine::Scene::LightComponent();
        lightComponent->lightType = RTBEngine::Rendering::LightType::Directional;
        lightComponent->color = RTBEngine::Math::Color(1.0f, 0.97f, 0.92f, 1.0f);
        lightComponent->intensity = 1.2f;
        lightComponent->syncPosition = false;
        lightComponent->syncDirection = true;
        lightObject->AddComponent(lightComponent);

        stagingScene->AddGameObject(lightObject, false);
        stagingScene->BringGameObjectToLife(lightObject);
    }

    bool PrefabEditorSession::Open(const std::filesystem::path& absolutePath)
    {
        Close();

        std::error_code ec;
        const std::filesystem::path canonicalPath =
            std::filesystem::weakly_canonical(absolutePath, ec);
        if (ec || !std::filesystem::exists(canonicalPath)) {
            RTB_ERROR("PrefabEditorSession: Prefab file not found: " + absolutePath.string());
            return false;
        }

        RTBEngine::Scene::Prefab* prefabAsset =
            RTBEngine::Scene::PrefabRegistry::GetInstance().GetByPath(canonicalPath.string());
        if (!prefabAsset) {
            RTB_ERROR("PrefabEditorSession: Prefab not registered: " + canonicalPath.string());
            return false;
        }

        stagingScene = std::make_unique<RTBEngine::Scene::Scene>("Prefab Edit");
        std::vector<RTBEngine::Scene::GameObject*> childGOs;
        rootObject = prefabAsset->Instantiate(nullptr, childGOs, false);
        if (!rootObject) {
            Close();
            RTB_ERROR("PrefabEditorSession: Failed to instantiate prefab: " + canonicalPath.string());
            return false;
        }

        stagingScene->AddGameObject(rootObject, false);
        for (RTBEngine::Scene::GameObject* child : childGOs) {
            if (child) {
                stagingScene->AddGameObject(child, false);
            }
        }
        stagingScene->BringGameObjectToLife(rootObject);
        SetupEditorLighting();

        assetPath = canonicalPath;
        isDirty = false;
        return true;
    }

    void PrefabEditorSession::Close()
    {
        rootObject = nullptr;
        stagingScene.reset();
        assetPath.clear();
        isDirty = false;
    }

    bool PrefabEditorSession::Save()
    {
        if (!IsOpen() || assetPath.empty()) {
            return false;
        }

        auto prefab = RTBEngine::Scene::Prefab::CreateFromGameObject(rootObject);
        if (!prefab) {
            RTB_ERROR("PrefabEditorSession: Failed to serialize prefab from staging scene.");
            return false;
        }

        if (!RTBEngine::Scripting::PrefabSaver::Save(*prefab, assetPath.string())) {
            RTB_ERROR("PrefabEditorSession: Failed to write prefab file: " + assetPath.string());
            return false;
        }

        RTBEngine::Scene::PrefabRegistry::GetInstance().Reload(prefab->GetName());
        isDirty = false;
        return true;
    }

}
