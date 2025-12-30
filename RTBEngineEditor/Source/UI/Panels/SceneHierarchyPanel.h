#pragma once

#include "EditorPanel.h"
#include <RTBEngine/ECS/GameObject.h>

namespace RTBEditor {

    class SceneHierarchyPanel : public EditorPanel {
    public:
        SceneHierarchyPanel();
        virtual ~SceneHierarchyPanel() override;

        virtual void OnUIRender(EditorContext& context) override;

    private:
        void DrawGameObjectNode(RTBEngine::ECS::GameObject* gameObject, EditorContext& context);

    private:
    };

}
