#include "BuildDialog.h"
#include <imgui.h>
#include <Windows.h>
#include <shobjidl.h>
#include "../../Project/Project.h"
#include <algorithm>
#include <cctype>
#include <filesystem>

namespace {
    namespace fs = std::filesystem;

    std::string NormalizeReferencePath(const std::string& path) {
        return fs::path(path).lexically_normal().generic_string();
    }

    std::string ToLower(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    }
}

namespace RTBEditor {

    BuildDialog::BuildDialog() {
        settings.gameName = "MyGame";
        settings.windowWidth = 1280;
        settings.windowHeight = 720;
        settings.fullscreen = false;
    }

    void BuildDialog::Open() {
        RefreshAvailableScenes();
        open = true;
    }

    void BuildDialog::Render() {
        if (open) {
            ImGui::OpenPopup("Build Settings");
        }

        if (ImGui::BeginPopupModal("Build Settings", &open, ImGuiWindowFlags_AlwaysAutoResize)) {

            static char nameBuf[256];
            // Initialize buffer once or when empty to avoid overwriting user input every frame
            if (settings.gameName != nameBuf) {
                 strcpy_s(nameBuf, sizeof(nameBuf), settings.gameName.c_str());
            }

            if (ImGui::InputText("Game Name", nameBuf, sizeof(nameBuf))) {
                settings.gameName = nameBuf;
            }

            static char pathBuf[1024];
            strcpy_s(pathBuf, sizeof(pathBuf), settings.outputDirectory.string().c_str());
            if (ImGui::InputText("Output Directory", pathBuf, sizeof(pathBuf))) {
                settings.outputDirectory = pathBuf;
            }
            ImGui::SameLine();
            if (ImGui::Button("Browse...")) {
                DrawDirectorySelector();
            }

            ImGui::Separator();

            ImGui::InputInt("Width", &settings.windowWidth);
            ImGui::InputInt("Height", &settings.windowHeight);
            ImGui::Checkbox("Fullscreen", &settings.fullscreen);

            ImGui::Separator();
            DrawStartSceneSelector();

            ImGui::Separator();

            if (isBuilding) {
                ImGui::ProgressBar(buildProgress, ImVec2(0.0f, 0.0f));
                ImGui::Text("%s", statusMessage.c_str());
            }

            ImGui::BeginDisabled(isBuilding);
            if (!IsSelectedStartSceneValid()) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Build")) {
                OnBuild();
            }
            if (!IsSelectedStartSceneValid()) {
                ImGui::EndDisabled();
            }
            ImGui::EndDisabled();

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                open = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (showResult) {
            ImGui::OpenPopup("Build Result");
        }

        bool showResultOpen = true; // Local bool for modal
        if (ImGui::BeginPopupModal("Build Result", &showResultOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", BuildSystem::GetResultMessage(lastResult).c_str());
            if (ImGui::Button("OK")) {
                showResult = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if (!showResultOpen) showResult = false;
    }

    void BuildDialog::DrawDirectorySelector() {
        IFileDialog* pFileDialog = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileDialog, (void**)&pFileDialog);

        if (SUCCEEDED(hr)) {
            DWORD options;
            pFileDialog->GetOptions(&options);
            pFileDialog->SetOptions(options | FOS_PICKFOLDERS);
            pFileDialog->SetTitle(L"Select Build Output Directory");

            if (SUCCEEDED(pFileDialog->Show(NULL))) {
                IShellItem* pItem;
                hr = pFileDialog->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr)) {
                        std::filesystem::path p(pszFilePath);
                        settings.outputDirectory = p;
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileDialog->Release();
        }
    }

    void BuildDialog::RefreshAvailableScenes() {
        availableScenes.clear();
        selectedStartSceneIndex = -1;
        selectedStartScene.clear();

        Project* project = Project::GetActiveProject();
        if (!project) {
            settings.startScene.clear();
            return;
        }

        settings.startScene = NormalizeReferencePath(project->GetStartScene());

        const fs::path scenesRoot = project->GetAssetRootPath() / "Scenes";
        if (fs::exists(scenesRoot)) {
            std::error_code ec;
            for (const auto& entry : fs::recursive_directory_iterator(scenesRoot, ec)) {
                if (ec) {
                    break;
                }

                if (!entry.is_regular_file()) {
                    continue;
                }

                if (ToLower(entry.path().extension().string()) != ".lua") {
                    continue;
                }

                std::error_code relError;
                const fs::path relativeToAssets = fs::relative(entry.path(), project->GetAssetRootPath(), relError);
                if (relError) {
                    continue;
                }

                availableScenes.push_back(project->GetAssetReferencePath(relativeToAssets));
            }
        }

        std::sort(availableScenes.begin(), availableScenes.end());

        auto selectedIt = std::find(availableScenes.begin(), availableScenes.end(), settings.startScene);
        if (selectedIt == availableScenes.end() && !availableScenes.empty()) {
            selectedIt = availableScenes.begin();
        }

        if (selectedIt != availableScenes.end()) {
            selectedStartSceneIndex = static_cast<int>(std::distance(availableScenes.begin(), selectedIt));
            selectedStartScene = *selectedIt;
            settings.startScene = selectedStartScene;
        }
    }

    bool BuildDialog::IsSelectedStartSceneValid() const {
        const Project* project = Project::GetActiveProject();
        if (!project || selectedStartScene.empty()) {
            return false;
        }

        const std::string normalized = NormalizeReferencePath(selectedStartScene);
        const std::string scenesPrefix = project->GetAssetReferencePath(fs::path("Scenes"));
        if (normalized.rfind(scenesPrefix + "/", 0) != 0) {
            return false;
        }

        if (ToLower(fs::path(normalized).extension().string()) != ".lua") {
            return false;
        }

        const fs::path absoluteScenePath = (project->GetProjectDirectory() / normalized).lexically_normal();
        return fs::exists(absoluteScenePath) && fs::is_regular_file(absoluteScenePath);
    }

    void BuildDialog::DrawStartSceneSelector() {
        ImGui::Text("Start Scene");

        if (availableScenes.empty()) {
            ImGui::TextDisabled("No scenes found in Assets/Scenes.");
            return;
        }

        const char* preview = selectedStartScene.empty() ? "Select scene" : selectedStartScene.c_str();
        if (ImGui::BeginCombo("##StartScene", preview)) {
            for (int i = 0; i < static_cast<int>(availableScenes.size()); ++i) {
                const bool selected = i == selectedStartSceneIndex;
                if (ImGui::Selectable(availableScenes[i].c_str(), selected)) {
                    selectedStartSceneIndex = i;
                    selectedStartScene = availableScenes[i];
                    settings.startScene = selectedStartScene;
                }
                if (selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (!IsSelectedStartSceneValid()) {
            ImGui::TextDisabled("Select a valid .lua scene under Assets/Scenes.");
        }
    }

    void BuildDialog::OnBuild() {
        if (settings.outputDirectory.empty()) {
            lastResult = BuildResult::InvalidOutputDirectory;
            showResult = true;
            return;
        }

        if (!IsSelectedStartSceneValid()) {
            lastResult = BuildResult::InvalidStartScene;
            showResult = true;
            return;
        }

        settings.startScene = NormalizeReferencePath(selectedStartScene);

        Project* project = Project::GetActiveProject();
        if (!project) {
            lastResult = BuildResult::NoProjectLoaded;
            showResult = true;
            return;
        }

        project->SetStartScene(settings.startScene);
        if (!project->Save()) {
            lastResult = BuildResult::ConfigWriteFailed;
            showResult = true;
            return;
        }

        isBuilding = true;
        // Run synchronously for now
        lastResult = BuildSystem::Build(settings, [this](const std::string& status, float progress) {
            statusMessage = status;
            buildProgress = progress;
            // Force redraw? Not possible in sync on main thread.
        });

        isBuilding = false;
        open = false;
        showResult = true;
    }

}
