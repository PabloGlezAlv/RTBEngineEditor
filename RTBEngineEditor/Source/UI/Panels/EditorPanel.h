#pragma once
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scene/SceneManager.h>
#include "../../Core/EditorTypes.h"
#include "../Prefab/PrefabEditorSession.h"
#include <filesystem>
#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>

namespace RTBEngine {
    namespace Core {
        class Window;
    }
}

namespace RTBEditor {

    namespace RTBEngineCore = RTBEngine::Core;

    struct StatsData {
        float fps = 0.0f;
        float frameTimeMs = 0.0f;
        uint32_t drawCalls = 0;
        uint32_t triangles = 0;
        uint32_t culledObjects = 0;
        uint32_t gameObjects = 0;
        uint32_t components = 0;
        uint32_t physicsBodies = 0;
        uint32_t audioSources = 0;
        uint32_t ecsProjectileCount = 0;
        float ecsProjectileSimMs = 0.0f;
    };

    struct NavDebugSettings {
        bool enabled = false;
        bool showBounds = true;
        bool showWalkableCells = true;
        bool showBlockedCells = true;
        bool showAgentPaths = true;
        int gridCellStep = 0;
        float yOffset = 0.15f;
    };

    struct OptionalWindowState {
        bool online = false;
        bool physicsLayers = false;
        bool navigationDebug = false;
    };

    struct EditorContext {
        RTBEngine::Scene::GameObject* selectedGameObject = nullptr;
        std::vector<RTBEngine::Scene::GameObject*> selectedGameObjects;
        EditorState state = EditorState::Edit;
        std::filesystem::path selectedAssetPath;
        std::filesystem::path pendingSceneLoad;
        std::filesystem::path pendingPrefabOpen;
        std::unique_ptr<PrefabEditorSession> prefabEditor;
        StatsData stats;
        bool showStatsOverlay = false;
        NavDebugSettings navDebug;
        OptionalWindowState optionalWindows;
        RTBEngineCore::Window* window = nullptr;

        std::function<void()> onCopySelection;
        std::function<void()> onPasteSelection;
        std::function<void()> onDuplicateSelection;
        std::function<bool()> canCopySelection;
        std::function<bool()> hasClipboardContent;
        std::function<bool()> ensureScenePhysicsReady;
    };

    inline void ClearSelection(EditorContext& context) {
        context.selectedGameObject = nullptr;
        context.selectedGameObjects.clear();
    }

    inline bool IsGameObjectInScene(const RTBEngine::Scene::Scene* scene,
                                    const RTBEngine::Scene::GameObject* gameObject) {
        if (!scene || !gameObject) {
            return false;
        }

        const auto& gameObjects = scene->GetGameObjects();
        return std::any_of(gameObjects.begin(), gameObjects.end(),
            [gameObject](const std::unique_ptr<RTBEngine::Scene::GameObject>& obj) {
                return obj.get() == gameObject;
            });
    }

    inline void PruneSelectionToScene(EditorContext& context,
                                      const RTBEngine::Scene::Scene* scene) {
        if (!scene) {
            ClearSelection(context);
            return;
        }

        context.selectedGameObjects.erase(
            std::remove_if(
                context.selectedGameObjects.begin(),
                context.selectedGameObjects.end(),
                [scene](RTBEngine::Scene::GameObject* gameObject) {
                    return !IsGameObjectInScene(scene, gameObject);
                }),
            context.selectedGameObjects.end());

        if (context.selectedGameObject &&
            !IsGameObjectInScene(scene, context.selectedGameObject)) {
            context.selectedGameObject = nullptr;
        }

        if (!context.selectedGameObject && !context.selectedGameObjects.empty()) {
            context.selectedGameObject = context.selectedGameObjects.front();
        }

        if (context.selectedGameObject &&
            std::find(context.selectedGameObjects.begin(),
                      context.selectedGameObjects.end(),
                      context.selectedGameObject) == context.selectedGameObjects.end()) {
            context.selectedGameObjects.push_back(context.selectedGameObject);
        }
    }

    inline void SetSingleSelection(EditorContext& context, RTBEngine::Scene::GameObject* gameObject) {
        context.selectedGameObjects.clear();
        context.selectedGameObject = gameObject;
        if (gameObject) {
            context.selectedGameObjects.push_back(gameObject);
        }
    }

    inline bool IsPrefabEditMode(const EditorContext& context) {
        return context.prefabEditor && context.prefabEditor->IsOpen();
    }

    inline RTBEngine::Scene::Scene* GetEditingScene(const EditorContext& context) {
        if (IsPrefabEditMode(context)) {
            return context.prefabEditor->GetStagingScene();
        }
        return RTBEngine::Scene::SceneManager::GetInstance().GetActiveScene();
    }

    inline void MarkEditingDirty(EditorContext& context) {
        if (IsPrefabEditMode(context)) {
            context.prefabEditor->MarkDirty();
            return;
        }
        RTBEngine::Scene::SceneManager::GetInstance().MarkSceneDirty();
    }

    inline void ToggleSelection(EditorContext& context, RTBEngine::Scene::GameObject* gameObject) {
        if (!gameObject) {
            ClearSelection(context);
            return;
        }

        auto it = std::find(context.selectedGameObjects.begin(), context.selectedGameObjects.end(), gameObject);
        if (it != context.selectedGameObjects.end()) {
            context.selectedGameObjects.erase(it);
        } else {
            context.selectedGameObjects.push_back(gameObject);
        }

        context.selectedGameObject = context.selectedGameObjects.empty()
            ? nullptr
            : context.selectedGameObjects.front();
    }

    class EditorPanel {
    public:
        virtual ~EditorPanel() = default;
        virtual void OnUIRender(EditorContext& context) = 0;
    };

}
