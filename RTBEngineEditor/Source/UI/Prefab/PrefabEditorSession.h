#pragma once

#include <RTBEngine/Scene/Scene.h>
#include <filesystem>
#include <memory>

namespace RTBEngine {
    namespace ECS {
        class GameObject;
    }
}

namespace RTBEditor {

    class PrefabEditorSession {
    public:
        static constexpr const char* EditorUtilityLightName = "__PrefabEditorLight";
        static bool IsEditorUtilityObject(const RTBEngine::ECS::GameObject* gameObject);

        bool IsOpen() const { return stagingScene != nullptr && rootObject != nullptr; }
        bool IsDirty() const { return isDirty; }
        void MarkDirty() { isDirty = true; }
        void ClearDirty() { isDirty = false; }

        const std::filesystem::path& GetAssetPath() const { return assetPath; }
        RTBEngine::ECS::Scene* GetStagingScene() const { return stagingScene.get(); }
        RTBEngine::ECS::GameObject* GetRootObject() const { return rootObject; }

        bool Open(const std::filesystem::path& absolutePath);
        void Close();
        bool Save();

    private:
        void SetupEditorLighting();

        std::filesystem::path assetPath;
        std::unique_ptr<RTBEngine::ECS::Scene> stagingScene;
        RTBEngine::ECS::GameObject* rootObject = nullptr;
        bool isDirty = false;
    };

}
