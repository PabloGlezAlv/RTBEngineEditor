#pragma once

#include "EditorPanel.h"
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <memory>
#include <string>
#include <array>
#include <filesystem>

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
        std::string FormatTypeName(const char* typeName);

        void DrawCubemapAssetInspector(const std::filesystem::path& cubemapPath);
        void SaveCubemapAsset(const std::filesystem::path& cubemapPath);

        std::vector<RTBEngine::ECS::Component*> componentsToRemove;
        std::unique_ptr<AssetBrowserModal> assetBrowserModal;

        // Cached euler angles in degrees to avoid quaternion round-trip every frame
        RTBEngine::ECS::GameObject* cachedRotationTarget = nullptr;
        RTBEngine::Math::Vector3 cachedRotationDeg;

        // Cached state for the cubemap asset editor
        std::filesystem::path cubemapEditorPath;
        // face order: right, left, top, bottom, front, back
        std::array<std::string, 6> cubemapFaces;
    };

}
