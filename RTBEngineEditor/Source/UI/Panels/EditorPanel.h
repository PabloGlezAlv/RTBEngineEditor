#pragma once
#include <RTBEngine/ECS/GameObject.h>
#include "../../Core/EditorTypes.h"

namespace RTBEditor {

    struct EditorContext {
        RTBEngine::ECS::GameObject* selectedGameObject = nullptr;
        EditorState state = EditorState::Edit;
    };

    class EditorPanel {
    public:
        virtual ~EditorPanel() = default;
        virtual void OnUIRender(EditorContext& context) = 0;
    };

}
