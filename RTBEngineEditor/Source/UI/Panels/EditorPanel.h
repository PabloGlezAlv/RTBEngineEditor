#pragma once
#include <RTBEngine/ECS/GameObject.h>

namespace RTBEditor {

    struct EditorContext {
        RTBEngine::ECS::GameObject* selectedGameObject = nullptr;
    };

    class EditorPanel {
    public:
        virtual ~EditorPanel() = default;
        virtual void OnUIRender(EditorContext& context) = 0;
    };

}
