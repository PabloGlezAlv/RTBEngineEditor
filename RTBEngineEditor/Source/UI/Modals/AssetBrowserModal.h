#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <vector>

namespace RTBEditor {

    enum class AssetType {
        Texture,
        Mesh,
        AudioClip,
        Font,
        Any
    };

    class AssetBrowserModal {
    public:
        AssetBrowserModal();
        ~AssetBrowserModal();

        // Open the modal with a filter
        void Open(AssetType type, std::function<void(const std::string&)> onAssetSelected);

        // Render the modal (call every frame)
        void Render();

        // Check if modal is open
        bool IsOpen() const { return isOpen; }

    private:
        bool isOpen = false;
        AssetType filterType = AssetType::Any;
        std::function<void(const std::string&)> callback;

        std::filesystem::path currentDirectory;
        std::filesystem::path rootDirectory;

        // Helper to check if file matches filter
        bool MatchesFilter(const std::filesystem::path& path);

        // Get file extensions for current filter
        std::vector<std::string> GetFilterExtensions();
    };

}
