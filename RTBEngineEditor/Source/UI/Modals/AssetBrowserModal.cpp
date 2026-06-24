#include "AssetBrowserModal.h"
#include <imgui.h>
#include "../../Project/Project.h"
#include <algorithm>
#include <cctype>

namespace RTBEditor {

    AssetBrowserModal::AssetBrowserModal() {
        if (Project::GetActiveProject()) {
            assetsDirectory = Project::GetActiveProject()->GetAssetRootPath();
        } else {
            assetsDirectory = "Assets";
        }
        defaultDirectory = "Default";
        rootDirectory = assetsDirectory; 
        currentDirectory = assetsDirectory;
        atRoot = true;
    }


    AssetBrowserModal::~AssetBrowserModal() {}

    void AssetBrowserModal::Open(AssetType type, std::function<void(const std::string&)> onAssetSelected,
        std::function<void(const std::string&)> onDefaultAssetSelected) {
        if (Project::GetActiveProject()) {
            assetsDirectory = Project::GetActiveProject()->GetAssetRootPath();
        } else {
            assetsDirectory = "Assets";
        }
        rootDirectory = assetsDirectory;
        filterType = type;
        callback = onAssetSelected;
        defaultCallback = onDefaultAssetSelected;
        currentDirectory = rootDirectory;
        isOpen = true;
        atRoot = true;

        if (filterType == AssetType::Scene) {
            const std::filesystem::path scenesDirectory = assetsDirectory / "Scenes";
            if (std::filesystem::exists(scenesDirectory) &&
                std::filesystem::is_directory(scenesDirectory)) {
                currentDirectory = scenesDirectory;
                atRoot = false;
            }
        }

        if (filterType == AssetType::Prefab) {
            const std::filesystem::path prefabsDirectory = assetsDirectory / "Prefabs";
            if (std::filesystem::exists(prefabsDirectory) &&
                std::filesystem::is_directory(prefabsDirectory)) {
                currentDirectory = prefabsDirectory;
                atRoot = false;
            }
        }

        if (filterType == AssetType::DataAsset) {
            const std::filesystem::path dataDirectory = assetsDirectory / "Data";
            if (std::filesystem::exists(dataDirectory) &&
                std::filesystem::is_directory(dataDirectory)) {
                currentDirectory = dataDirectory;
                atRoot = false;
            }
        }
    }


    std::vector<std::string> AssetBrowserModal::GetFilterExtensions() {
        switch (filterType) {
        case AssetType::Texture:
            return { ".png", ".jpg", ".jpeg", ".tga", ".dds", ".bmp", ".texture" };
        case AssetType::Fbx:
            return { ".fbx" };
        case AssetType::Mesh:
            return { ".fbx", ".obj", ".gltf", ".glb" };
        case AssetType::AudioClip:
            return { ".wav", ".mp3", ".ogg" };
        case AssetType::Font:
            return { ".ttf", ".otf" };
        case AssetType::Scene:
            return { ".lua" };
        case AssetType::Prefab:
            return { ".prefab" };
        case AssetType::DataAsset:
            return { ".rtbasset" };
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

            // Back button and path
            if (!atRoot) {
                bool atFirstLevel = (currentDirectory == assetsDirectory || currentDirectory == defaultDirectory);
                if (ImGui::Button("<- Back")) {
                    if (atFirstLevel) {
                        atRoot = true;
                    } else {
                        currentDirectory = currentDirectory.parent_path();
                    }
                }
                ImGui::SameLine();
                ImGui::Text("Path: %s", currentDirectory.string().c_str());
            } else {
                ImGui::Text("Select source:");
            }
            ImGui::Separator();

            // Filter info
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Filter: %s",
                filterType == AssetType::Texture ? "Textures" :
                filterType == AssetType::Fbx ? "FBX Files" :
                filterType == AssetType::Mesh ? "Meshes" :
                filterType == AssetType::AudioClip ? "Audio Clips" :
                filterType == AssetType::Scene ? "Scenes" :
                filterType == AssetType::Prefab ? "Prefabs" :
                filterType == AssetType::DataAsset ? "Data Assets" :
                filterType == AssetType::Font ? "Fonts" : "All Files");
            ImGui::Separator();

            // File list
            ImGui::BeginChild("FileList", ImVec2(0, -30), true);

            if (atRoot) {
                if (ImGui::Selectable("[D] Assets", false, ImGuiSelectableFlags_AllowDoubleClick)) {
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        currentDirectory = assetsDirectory;
                        atRoot = false;
                    }
                }
                if (ImGui::Selectable("[D] Default", false, ImGuiSelectableFlags_AllowDoubleClick)) {
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        currentDirectory = defaultDirectory;
                        atRoot = false;
                    }
                }
            } else {
                bool inDefault = currentDirectory.string().find(defaultDirectory.string()) == 0;

                if (std::filesystem::exists(currentDirectory)) {
                    for (auto& entry : std::filesystem::directory_iterator(currentDirectory)) {
                        if (!MatchesFilter(entry.path())) continue;

                        std::string filename = entry.path().filename().string();
                        bool isDirectory = entry.is_directory();

                        if (isDirectory) {
                            if (ImGui::Selectable(("[D] " + filename).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                                if (ImGui::IsMouseDoubleClicked(0)) {
                                    currentDirectory /= entry.path().filename();
                                }
                            }
                        } else {
                            if (ImGui::Selectable(("[F] " + filename).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                                if (ImGui::IsMouseDoubleClicked(0)) {
                                    if (inDefault) {
                                        // Reconstruct "Default/subpath/file" from the current path string
                                        std::string cur = currentDirectory.string();
                                        std::string def = defaultDirectory.string();
                                        std::string sub = cur.size() > def.size() ? cur.substr(def.size() + 1) + "/" : "";
                                        std::string relStr = def + "/" + (sub.empty() ? "" : sub) + filename;
                                        std::replace(relStr.begin(), relStr.end(), '\\', '/');
                                        if (defaultCallback) defaultCallback(relStr);
                                    } else {
                                        std::filesystem::path rel = std::filesystem::relative(entry.path(), assetsDirectory);
                                        std::string relStr = rel.string();
                                        std::replace(relStr.begin(), relStr.end(), '\\', '/');
                                        if (callback) callback(relStr);
                                    }
                                    isOpen = false;
                                    ImGui::CloseCurrentPopup();
                                }
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
