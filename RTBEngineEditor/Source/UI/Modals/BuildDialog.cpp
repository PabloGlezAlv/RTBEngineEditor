#include "BuildDialog.h"
#include <imgui.h>
#include <Windows.h>
#include <shobjidl.h>
#include <iostream>

namespace RTBEditor {

    BuildDialog::BuildDialog() {
        m_Settings.gameName = "MyGame";
        m_Settings.windowWidth = 1280;
        m_Settings.windowHeight = 720;
        m_Settings.fullscreen = false;
    }

    void BuildDialog::Open() {
        m_Open = true;
    }

    void BuildDialog::Render() {
        if (m_Open) {
            ImGui::OpenPopup("Build Settings");
        }

        if (ImGui::BeginPopupModal("Build Settings", &m_Open, ImGuiWindowFlags_AlwaysAutoResize)) {

            static char nameBuf[256];
            // Initialize buffer once or when empty to avoid overwriting user input every frame
            if (m_Settings.gameName != nameBuf) {
                 strcpy_s(nameBuf, m_Settings.gameName.c_str());
            }

            if (ImGui::InputText("Game Name", nameBuf, sizeof(nameBuf))) {
                m_Settings.gameName = nameBuf;
            }

            static char pathBuf[1024];
            strcpy_s(pathBuf, m_Settings.outputDirectory.string().c_str());
            if (ImGui::InputText("Output Directory", pathBuf, sizeof(pathBuf))) {
                m_Settings.outputDirectory = pathBuf;
            }
            ImGui::SameLine();
            if (ImGui::Button("Browse...")) {
                DrawDirectorySelector();
            }

            ImGui::Separator();

            ImGui::InputInt("Width", &m_Settings.windowWidth);
            ImGui::InputInt("Height", &m_Settings.windowHeight);
            ImGui::Checkbox("Fullscreen", &m_Settings.fullscreen);

            ImGui::Separator();

            if (m_IsBuilding) {
                ImGui::ProgressBar(m_BuildProgress, ImVec2(0.0f, 0.0f));
                ImGui::Text("%s", m_StatusMessage.c_str());
            }

            ImGui::BeginDisabled(m_IsBuilding);
            if (ImGui::Button("Build")) {
                OnBuild();
            }
            ImGui::EndDisabled();

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                m_Open = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (m_ShowResult) {
            ImGui::OpenPopup("Build Result");
        }

        bool showResultOpen = true; // Local bool for modal
        if (ImGui::BeginPopupModal("Build Result", &showResultOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", BuildSystem::GetResultMessage(m_LastResult).c_str());
            if (ImGui::Button("OK")) {
                m_ShowResult = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if (!showResultOpen) m_ShowResult = false;
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
                        m_Settings.outputDirectory = p;
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileDialog->Release();
        }
    }

    void BuildDialog::OnBuild() {
        if (m_Settings.outputDirectory.empty()) {
            m_LastResult = BuildResult::InvalidOutputDirectory;
            m_ShowResult = true;
            return;
        }

        m_IsBuilding = true;
        // Run synchronously for now
        m_LastResult = BuildSystem::Build(m_Settings, [this](const std::string& status, float progress) {
            m_StatusMessage = status;
            m_BuildProgress = progress;
            // Force redraw? Not possible in sync on main thread.
        });

        m_IsBuilding = false;
        m_Open = false;
        m_ShowResult = true;
    }

}
