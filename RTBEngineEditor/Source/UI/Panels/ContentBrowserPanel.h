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
        Shader
    };

    class ContentBrowserPanel : public EditorPanel {
    public:
        ContentBrowserPanel();
        virtual ~ContentBrowserPanel() override;

        virtual void OnUIRender(EditorContext& context) override;

    private:
        void LoadIcons();
        RTBEngine::Rendering::Texture* GetIconForFile(const std::filesystem::path& path);

    private:
        std::filesystem::path currentDirectory;
        std::map<IconType, RTBEngine::Rendering::Texture*> icons;
    };

}
