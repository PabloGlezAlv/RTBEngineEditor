#pragma once
#include <RTBEngine/ECS/GameObject.h>
#include "../../Core/EditorTypes.h"
#include <filesystem>
#include <string>

namespace RTBEditor {

    struct EditorContext {
        RTBEngine::ECS::GameObject* selectedGameObject = nullptr;
        EditorState state = EditorState::Edit;
        std::filesystem::path selectedAssetPath;

        std::filesystem::path pendingSceneLoad;
    };

    class EditorPanel {
    public:
        virtual ~EditorPanel() = default;
        virtual void OnUIRender(EditorContext& context) = 0;
    };

}
