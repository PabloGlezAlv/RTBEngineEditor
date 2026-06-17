#pragma once
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/Scene.h>
#include "../../Core/EditorTypes.h"
#include <filesystem>
#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <functional>

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
        RTBEngine::ECS::GameObject* selectedGameObject = nullptr;
        std::vector<RTBEngine::ECS::GameObject*> selectedGameObjects;
        EditorState state = EditorState::Edit;
        std::filesystem::path selectedAssetPath;
        std::filesystem::path pendingSceneLoad;
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

    inline bool IsGameObjectInScene(const RTBEngine::ECS::Scene* scene,
                                    const RTBEngine::ECS::GameObject* gameObject) {
        if (!scene || !gameObject) {
            return false;
        }

        const auto& gameObjects = scene->GetGameObjects();
        return std::any_of(gameObjects.begin(), gameObjects.end(),
            [gameObject](const std::unique_ptr<RTBEngine::ECS::GameObject>& obj) {
                return obj.get() == gameObject;
            });
    }

    inline void PruneSelectionToScene(EditorContext& context,
                                      const RTBEngine::ECS::Scene* scene) {
        if (!scene) {
            ClearSelection(context);
            return;
        }

        context.selectedGameObjects.erase(
            std::remove_if(
                context.selectedGameObjects.begin(),
                context.selectedGameObjects.end(),
                [scene](RTBEngine::ECS::GameObject* gameObject) {
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

    inline void SetSingleSelection(EditorContext& context, RTBEngine::ECS::GameObject* gameObject) {
        context.selectedGameObjects.clear();
        context.selectedGameObject = gameObject;
        if (gameObject) {
            context.selectedGameObjects.push_back(gameObject);
        }
    }

    inline void ToggleSelection(EditorContext& context, RTBEngine::ECS::GameObject* gameObject) {
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
