#pragma once

#include "EditorPanel.h"
#include <RTBEngine/Math/Vectors/Vector3.h>
#include <RTBEngine/Math/Quaternions/Quaternion.h>
#include <memory>
#include <string>
#include <array>
#include <filesystem>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
        class Component;
        class ParticleSystem;
        class NavGridComponent;
    }
    namespace Reflection {
        struct PropertyInfo;
    }
    namespace Animation {
        class Animator;
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
        void DrawComponents(RTBEngine::ECS::GameObject* gameObject, EditorContext& context);
        void DrawProperty(RTBEngine::ECS::Component* component, const RTBEngine::Reflection::PropertyInfo& prop);
        bool DrawSceneStringProperty(RTBEngine::ECS::Component* component,
                                     const RTBEngine::Reflection::PropertyInfo& prop,
                                     std::string* value);
        bool DrawAssetRefProperty(RTBEngine::ECS::Component* component,
                                  const RTBEngine::Reflection::PropertyInfo& prop,
                                  std::string* value);
        std::string FormatTypeName(const char* typeName);

        void DrawAnimatorComponent(RTBEngine::Animation::Animator* animator);
        void DrawParticleSystemComponent(RTBEngine::ECS::ParticleSystem* particleSystem);
        void DrawNavGridComponent(RTBEngine::ECS::NavGridComponent* navGridComponent, EditorContext& context);
        void DrawCubemapAssetInspector(const std::filesystem::path& cubemapPath);
        void SaveCubemapAsset(const std::filesystem::path& cubemapPath);
        void DrawScriptPreview(const std::filesystem::path& scriptPath);
        void DrawFbxAssetInspector(const std::filesystem::path& fbxPath);
        void DrawTextureAssetInspector(const std::filesystem::path& texturePath);
        void SaveTextureAsset(const std::filesystem::path& texturePath);

        std::vector<RTBEngine::ECS::Component*> componentsToRemove;
        std::unique_ptr<AssetBrowserModal> assetBrowserModal;

        // Cached euler angles in degrees to avoid quaternion round-trip every frame
        RTBEngine::ECS::GameObject* cachedRotationTarget = nullptr;
        RTBEngine::Math::Vector3 cachedRotationDeg;
        RTBEngine::Math::Quaternion cachedRotationSource = RTBEngine::Math::Quaternion::Identity();

        // Cached state for the cubemap asset editor
        std::filesystem::path cubemapEditorPath;
        // face order: right, left, top, bottom, front, back
        std::array<std::string, 6> cubemapFaces;

        // Cached state for the script preview
        std::filesystem::path scriptPreviewPath;
        std::string scriptPreviewContent;

        // Cached state for the texture asset editor
        std::filesystem::path textureEditorPath;
        std::string textureAssetImage;
        bool textureAssetFlip = true;

        // Cached state for animator compatibility scans
        RTBEngine::Animation::Animator* animatorScanTarget = nullptr;
        std::string animatorScanStatus;

        char addComponentSearchBuffer[128] = {};
        bool addComponentPopupWasOpen = false;

    };

}
