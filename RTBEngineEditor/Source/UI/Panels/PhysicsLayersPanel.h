#pragma once

#include "EditorPanel.h"
#include <RTBEngine/Physics/PhysicsLayerSettings.h>

#include <vector>

namespace RTBEditor {

    class PhysicsLayersPanel : public EditorPanel {
    public:
        PhysicsLayersPanel();
        ~PhysicsLayersPanel() override;

        PhysicsLayersPanel(const PhysicsLayersPanel&) = delete;
        PhysicsLayersPanel& operator=(const PhysicsLayersPanel&) = delete;

        void OnUIRender(EditorContext& context) override;

    private:
        void LoadFromProject();
        bool SaveToProject();
        void DrawLayerNames();
        void DrawCollisionMatrix();
        void DrawCollisionMatrixToolbar(int visibleLayerCount);
        void RefreshActivePhysicsFilters();
        bool IsLayerVisible(int layerIndex) const;
        void BuildVisibleLayerIndices(std::vector<int>& outIndices) const;
        static const char* AbbreviateLayerLabel(const char* fullName, char* outBuffer, size_t bufferSize);

        int layerCount = 1;
        char layerFilter[64] = {};
        float matrixCellSize = 22.0f;
        bool compactTriangleView = true;
        bool showLayerIndices = false;
        char layerNameBuffers[RTBEngine::Physics::PhysicsLayerSettings::MaxLayers][48] = {};
        bool collisionMatrix[RTBEngine::Physics::PhysicsLayerSettings::MaxLayers][RTBEngine::Physics::PhysicsLayerSettings::MaxLayers] = {};
        std::string lastMessage;
        bool lastSaveSucceeded = true;
    };

}
