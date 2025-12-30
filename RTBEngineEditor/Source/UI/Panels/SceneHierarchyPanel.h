#pragma once

#include "EditorPanel.h"
#include <RTBEngine/ECS/GameObject.h>

namespace RTBEditor {

    class SceneHierarchyPanel : public EditorPanel {
    public:
        SceneHierarchyPanel();
        virtual ~SceneHierarchyPanel() override;

        virtual void OnUIRender() override;

        RTBEngine::ECS::GameObject* GetSelectedGameObject() const { return selectedContext; }
        void SetSelectedGameObject(RTBEngine::ECS::GameObject* gameObject) { selectedContext = gameObject; }

    private:
        void DrawGameObjectNode(RTBEngine::ECS::GameObject* gameObject);

    private:
        RTBEngine::ECS::GameObject* selectedContext = nullptr;
    };

}
