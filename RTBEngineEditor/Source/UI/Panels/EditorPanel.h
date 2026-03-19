#pragma once
#include <RTBEngine/ECS/GameObject.h>
#include "../../Core/EditorTypes.h"
#include <filesystem>
#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>

namespace RTBEditor {

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

    struct EditorContext {
        RTBEngine::ECS::GameObject* selectedGameObject = nullptr;
        std::vector<RTBEngine::ECS::GameObject*> selectedGameObjects;
        EditorState state = EditorState::Edit;
        std::filesystem::path selectedAssetPath;
        std::filesystem::path pendingSceneLoad;
        StatsData stats;
        bool showStatsOverlay = false;
    };

    inline void ClearSelection(EditorContext& context) {
        context.selectedGameObject = nullptr;
        context.selectedGameObjects.clear();
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
