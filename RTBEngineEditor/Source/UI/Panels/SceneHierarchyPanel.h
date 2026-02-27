#pragma once

#include "EditorPanel.h"
#include <RTBEngine/ECS/GameObject.h>
#include <RTBEngine/ECS/Scene.h>
#include <vector>

namespace RTBEditor {

    class SceneHierarchyPanel : public EditorPanel {
    public:
        SceneHierarchyPanel();
        virtual ~SceneHierarchyPanel() override;

        virtual void OnUIRender(EditorContext& context) override;

    private:
        void DrawGameObjectNode(RTBEngine::ECS::GameObject* gameObject, EditorContext& context);

        //GameObject creation
        void CreateSphere(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);
        void CreateCube(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);
        void CreatePlane(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);

        //UI creation
        void CreateCanvas(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);
        void CreateUIButton(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);
        void CreateUIText(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);

        void CollectDescendants(RTBEngine::ECS::GameObject* gameObject, std::vector<RTBEngine::ECS::GameObject*>& out);
        void DeleteGameObject(RTBEngine::ECS::Scene* scene, RTBEngine::ECS::GameObject* gameObject, EditorContext& context);
    };

}
