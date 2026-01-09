#pragma once

#include "EditorPanel.h"
#include <memory>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
        class Component;
    }
    namespace Reflection {
        struct PropertyInfo;
    }
}

namespace RTBEditor {
    class AssetBrowserModal;

    class InspectorPanel : public EditorPanel {
    public:
        InspectorPanel();
        virtual ~InspectorPanel() override;

        virtual void OnUIRender(EditorContext& context) override;

    private:
        void DrawComponents(RTBEngine::ECS::GameObject* gameObject);
        void DrawProperty(RTBEngine::ECS::Component* component, const RTBEngine::Reflection::PropertyInfo& prop);

        std::vector<RTBEngine::ECS::Component*> m_ComponentsToRemove;
        std::unique_ptr<AssetBrowserModal> m_AssetBrowserModal;
    };

}
