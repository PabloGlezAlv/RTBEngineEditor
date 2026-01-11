#include "BuildDialog.h"
#include <imgui.h>
#include <Windows.h>
#include <shobjidl.h>
#include <iostream>

namespace RTBEditor {

    BuildDialog::BuildDialog() {
        settings.gameName = "MyGame";
        settings.windowWidth = 1280;
        settings.windowHeight = 720;
        settings.fullscreen = false;
    }

    void BuildDialog::Open() {
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
                 strcpy_s(nameBuf, settings.gameName.c_str());
            }

            if (ImGui::InputText("Game Name", nameBuf, sizeof(nameBuf))) {
                settings.gameName = nameBuf;
            }

            static char pathBuf[1024];
            strcpy_s(pathBuf, settings.outputDirectory.string().c_str());
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

            if (isBuilding) {
                ImGui::ProgressBar(buildProgress, ImVec2(0.0f, 0.0f));
                ImGui::Text("%s", statusMessage.c_str());
            }

            ImGui::BeginDisabled(isBuilding);
            if (ImGui::Button("Build")) {
                OnBuild();
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

    void BuildDialog::OnBuild() {
        if (settings.outputDirectory.empty()) {
            lastResult = BuildResult::InvalidOutputDirectory;
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
