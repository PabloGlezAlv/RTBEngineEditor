#pragma once
#include "EditorPanel.h"
#include <filesystem>
#include <RTBEngine/Rendering/Texture.h>
#include <map>
#include <vector>
#include <string>

namespace RTBEditor {

    enum class IconType {
        Folder,
        File,
        Lua,
        Model,
        Image,
        Shader,
        Cubemap,
        Prefab
    };

    struct FbxEmbeddedTexture {
        RTBEngine::Rendering::Texture* texture = nullptr;
        std::string name;
        std::string diskPath;  // Relative path from asset root, e.g. "Models/walking_Mutant.png"
    };

    class ContentBrowserPanel : public EditorPanel {
    public:
        ContentBrowserPanel();
        virtual ~ContentBrowserPanel() override;

        virtual void OnUIRender(EditorContext& context) override;

    private:
        void LoadIcons();
        RTBEngine::Rendering::Texture* GetIconForFile(const std::filesystem::path& path);
        void DrawContextMenu();
        void LoadFbxTextures(const std::filesystem::path& fbxPath);

    private:
        std::filesystem::path currentDirectory;
        std::filesystem::path selectedPath;
        std::filesystem::path renamingPath;
        char renameBuffer[256] = {};
        std::map<IconType, RTBEngine::Rendering::Texture*> icons;

        // Expanded FBX sub-textures
        std::filesystem::path expandedFbxPath;
        std::vector<FbxEmbeddedTexture> expandedFbxTextures;
    };

}
