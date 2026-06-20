#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <vector>

namespace RTBEditor {

    enum class AssetType {
        Texture,
        Fbx,
        Mesh,
        AudioClip,
        Font,
        Scene,
        Prefab,
        Any
    };

    class AssetBrowserModal {
    public:
        AssetBrowserModal();
        ~AssetBrowserModal();

        // Open the modal with a filter
        void Open(AssetType type, std::function<void(const std::string&)> onAssetSelected, 
        std::function<void(const std::string&)> onDefaultAssetSelected = nullptr);

        // Render the modal (call every frame)
        void Render();

        // Check if modal is open
        bool IsOpen() const { return isOpen; }

    private:
        bool isOpen = false;
        AssetType filterType = AssetType::Any;
        std::function<void(const std::string&)> callback;
        std::function<void(const std::string&)> defaultCallback;
        
        bool atRoot = true;
        std::filesystem::path assetsDirectory;
        std::filesystem::path defaultDirectory;
        std::filesystem::path currentDirectory;
        std::filesystem::path rootDirectory;

        // Helper to check if file matches filter
        bool MatchesFilter(const std::filesystem::path& path);

        // Get file extensions for current filter
        std::vector<std::string> GetFilterExtensions();
    };

}
