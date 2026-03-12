#pragma once
#include <RTBEngine/ECS/GameObject.h>
#include "../../Core/EditorTypes.h"
#include <filesystem>
#include <string>
#include <cstdint>

namespace RTBEditor {

    struct StatsData {
        float fps = 0.0f;
        float frameTimeMs = 0.0f;
        uint32_t drawCalls = 0;
        uint32_t triangles = 0;
        uint32_t gameObjects = 0;
        uint32_t components = 0;
        uint32_t physicsBodies = 0;
        uint32_t audioSources = 0;
    };

    struct EditorContext {
        RTBEngine::ECS::GameObject* selectedGameObject = nullptr;
        EditorState state = EditorState::Edit;
        std::filesystem::path selectedAssetPath;
        std::filesystem::path pendingSceneLoad;
        StatsData stats;
        bool showStatsOverlay = false;
    };

    class EditorPanel {
    public:
        virtual ~EditorPanel() = default;
        virtual void OnUIRender(EditorContext& context) = 0;
    };

}
