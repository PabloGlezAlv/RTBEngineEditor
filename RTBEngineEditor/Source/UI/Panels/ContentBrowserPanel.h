#pragma once
#include "EditorPanel.h"
#include <filesystem>
#include <RTBEngine/Rendering/Texture.h>
#include <map>

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

    class ContentBrowserPanel : public EditorPanel {
    public:
        ContentBrowserPanel();
        virtual ~ContentBrowserPanel() override;

        virtual void OnUIRender(EditorContext& context) override;

    private:
        void LoadIcons();
        RTBEngine::Rendering::Texture* GetIconForFile(const std::filesystem::path& path);
        void DrawContextMenu();

    private:
        std::filesystem::path currentDirectory;
        std::filesystem::path selectedPath;
        std::filesystem::path renamingPath;
        char renameBuffer[256] = {};
        std::map<IconType, RTBEngine::Rendering::Texture*> icons;
    };

}
