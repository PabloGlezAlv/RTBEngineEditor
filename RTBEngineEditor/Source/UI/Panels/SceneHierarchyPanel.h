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
        void DrawGameObjectNode(RTBEngine::Scene::GameObject* gameObject, EditorContext& context);
        void DrawHierarchySearchResults(RTBEngine::Scene::Scene* scene,
                                        EditorContext& context,
                                        const char* filter);
        void UpdateHierarchyRevealTarget(RTBEngine::Scene::GameObject* gameObject);

        char hierarchySearchBuffer[128] = {};
        bool hierarchyWasFiltering = false;
        RTBEngine::Scene::GameObject* hierarchyPendingReveal = nullptr;
        std::unordered_set<RTBEngine::Scene::GameObject*> hierarchyForceOpenNodes;

        //GameObject creation
        void CreateSphere(RTBEngine::Scene::Scene* scene, EditorContext& context, RTBEngine::Scene::GameObject* parent = nullptr);
        void CreateCube(RTBEngine::Scene::Scene* scene, EditorContext& context, RTBEngine::Scene::GameObject* parent = nullptr);
        void CreatePlane(RTBEngine::Scene::Scene* scene, EditorContext& context, RTBEngine::Scene::GameObject* parent = nullptr);

        //UI creation
        void CreateCanvas(RTBEngine::Scene::Scene* scene, EditorContext& context, RTBEngine::Scene::GameObject* parent = nullptr);
        void CreateUIButton(RTBEngine::Scene::Scene* scene, EditorContext& context, RTBEngine::Scene::GameObject* parent = nullptr);
        void CreateUIInputField(RTBEngine::Scene::Scene* scene, EditorContext& context, RTBEngine::Scene::GameObject* parent = nullptr);
        void CreateUISlider(RTBEngine::Scene::Scene* scene, EditorContext& context, RTBEngine::Scene::GameObject* parent = nullptr);
        void CreateUIText(RTBEngine::Scene::Scene* scene, EditorContext& context, RTBEngine::Scene::GameObject* parent = nullptr);

        void CreateParticleSystem(RTBEngine::Scene::Scene* scene, EditorContext& context, RTBEngine::Scene::GameObject* parent = nullptr);

        void DeleteGameObject(RTBEngine::Scene::Scene* scene, RTBEngine::Scene::GameObject* gameObject, EditorContext& context);
    };

}
