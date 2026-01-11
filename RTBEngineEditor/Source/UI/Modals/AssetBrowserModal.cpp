#include "AssetBrowserModal.h"
#include <imgui.h>
#include "../../Project/Project.h"
#include <algorithm>
#include <cctype>

namespace RTBEditor {

    AssetBrowserModal::AssetBrowserModal() {
        if (Project::GetActiveProject()) {
            rootDirectory = Project::GetActiveProject()->GetAssetDirectory();
        }
        else {
            rootDirectory = "Assets";
        }
        currentDirectory = rootDirectory;
    }

    AssetBrowserModal::~AssetBrowserModal() {}

    void AssetBrowserModal::Open(AssetType type, std::function<void(const std::string&)> onAssetSelected) {
        filterType = type;
        callback = onAssetSelected;
        currentDirectory = rootDirectory;
        isOpen = true;
    }

    std::vector<std::string> AssetBrowserModal::GetFilterExtensions() {
        switch (filterType) {
        case AssetType::Texture:
            return { ".png", ".jpg", ".jpeg", ".tga", ".dds", ".bmp" };
        case AssetType::Mesh:
            return { ".fbx", ".obj", ".gltf", ".glb" };
        case AssetType::AudioClip:
            return { ".wav", ".mp3", ".ogg" };
        case AssetType::Font:
            return { ".ttf", ".otf" };
        case AssetType::Any:
        default:
            return {};
        }
    }

    bool AssetBrowserModal::MatchesFilter(const std::filesystem::path& path) {
        if (std::filesystem::is_directory(path)) return true;

        if (filterType == AssetType::Any) return true;

        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        auto extensions = GetFilterExtensions();
        return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
    }

    void AssetBrowserModal::Render() {
        if (!isOpen) return;
        
        if (isOpen && !ImGui::IsPopupOpen("Select Asset")) {
            ImGui::OpenPopup("Select Asset");
        }

        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

        if (ImGui::BeginPopupModal("Select Asset", &isOpen, ImGuiWindowFlags_NoCollapse)) {

            // Breadcrumbs
            if (currentDirectory != rootDirectory) {
                if (ImGui::Button("<- Back")) {
                    currentDirectory = currentDirectory.parent_path();
                }
                ImGui::SameLine();
            }
            ImGui::Text("Path: %s", std::filesystem::relative(currentDirectory, rootDirectory).string().c_str());
            ImGui::Separator();

            // Filter info
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Filter: %s",
                filterType == AssetType::Texture ? "Textures" :
                filterType == AssetType::Mesh ? "Meshes" :
                filterType == AssetType::AudioClip ? "Audio Clips" :
                filterType == AssetType::Font ? "Fonts" : "All Files");
            ImGui::Separator();

            // File list
            ImGui::BeginChild("FileList", ImVec2(0, -30), true);

            if (std::filesystem::exists(currentDirectory)) {
                for (auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
                    if (!MatchesFilter(entry.path())) continue;

                    std::string filename = entry.path().filename().string();
                    bool isDirectory = entry.is_directory();

                    if (isDirectory) {
                        if (ImGui::Selectable((" 📁 " + filename).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                            if (ImGui::IsMouseDoubleClicked(0)) {
                                currentDirectory /= entry.path().filename();
                            }
                        }
                    }
                    else {
                        if (ImGui::Selectable(("📄 " + filename).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                            if (ImGui::IsMouseDoubleClicked(0)) {
                                // Selected! Return relative path
                                std::string relativePath = std::filesystem::relative(entry.path(), rootDirectory).string();
                                if (callback) {
                                    callback(relativePath);
                                }
                                isOpen = false;
                                ImGui::CloseCurrentPopup();
                            }
                        }
                    }
                }
            }

            ImGui::EndChild();

            // Bottom buttons
            ImGui::Separator();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                isOpen = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

}
