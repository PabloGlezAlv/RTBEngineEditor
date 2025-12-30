#pragma once

#include "EditorPanel.h"

namespace RTBEditor {

    class InspectorPanel : public EditorPanel {
    public:
        InspectorPanel();
        virtual ~InspectorPanel() override;

        virtual void OnUIRender(EditorContext& context) override;

    private:
        void DrawComponents(RTBEngine::ECS::GameObject* gameObject);
    };

}
