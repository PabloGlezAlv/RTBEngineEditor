#pragma once

#include "EditorPanel.h"
#include <RTBEngine/Scene/GameObject.h>
#include <RTBEngine/Scene/Scene.h>
#include <unordered_set>
#include <vector>

namespace RTBEditor {

    class SceneHierarchyPanel : public EditorPanel {
    public:
        SceneHierarchyPanel();
        virtual ~SceneHierarchyPanel() override;

        virtual void OnUIRender(EditorContext& context) override;

    private:
        void DrawGameObjectNode(RTBEngine::ECS::GameObject* gameObject, EditorContext& context);
        void DrawHierarchySearchResults(RTBEngine::ECS::Scene* scene,
                                        EditorContext& context,
                                        const char* filter);
        void UpdateHierarchyRevealTarget(RTBEngine::ECS::GameObject* gameObject);

        char hierarchySearchBuffer[128] = {};
        bool hierarchyWasFiltering = false;
        RTBEngine::ECS::GameObject* hierarchyPendingReveal = nullptr;
        std::unordered_set<RTBEngine::ECS::GameObject*> hierarchyForceOpenNodes;

        //GameObject creation
        void CreateSphere(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);
        void CreateCube(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);
        void CreatePlane(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);

        //UI creation
        void CreateCanvas(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);
        void CreateUIButton(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);
        void CreateUIInputField(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);
        void CreateUISlider(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);
        void CreateUIText(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);

        void CreateParticleSystem(RTBEngine::ECS::Scene* scene, EditorContext& context, RTBEngine::ECS::GameObject* parent = nullptr);

        void DeleteGameObject(RTBEngine::ECS::Scene* scene, RTBEngine::ECS::GameObject* gameObject, EditorContext& context);
    };

}
