#include "ContentBrowserPanel.h"
#include <imgui.h>
#include <RTBEngine/Core/ResourceManager.h>
#include "../../Project/Project.h"
#include "../DragDropPayloads.h"

namespace RTBEditor {

    ContentBrowserPanel::ContentBrowserPanel() {
        if (Project::GetActiveProject()) {
            currentDirectory = Project::GetActiveProject()->GetAssetDirectory();
        } else {
            currentDirectory = "Assets";
        }

        if (!std::filesystem::exists(currentDirectory)) {
            std::filesystem::create_directories(currentDirectory);
        }

        LoadIcons();
    }

    ContentBrowserPanel::~ContentBrowserPanel() {}

    void ContentBrowserPanel::LoadIcons() {
        auto& rm = RTBEngine::Core::ResourceManager::GetInstance();
        icons[IconType::Folder]  = rm.LoadTexture("Default/Icons/folder.png");
        icons[IconType::File]    = rm.LoadTexture("Default/Icons/file.png");
        icons[IconType::Lua]     = rm.LoadTexture("Default/Icons/lua.png");
        icons[IconType::Model]   = rm.LoadTexture("Default/Icons/model.png");
        icons[IconType::Image]   = rm.LoadTexture("Default/Icons/image.png");
        icons[IconType::Shader]  = rm.LoadTexture("Default/Icons/shader.png");

        // Use folder icon as fallback if dedicated cubemap icon is not present
        RTBEngine::Rendering::Texture* cubemapIcon = rm.LoadTexture("Default/Icons/cubemap.png");
        icons[IconType::Cubemap] = cubemapIcon ? cubemapIcon : icons[IconType::Folder];
    }

    RTBEngine::Rendering::Texture* ContentBrowserPanel::GetIconForFile(const std::filesystem::path& path) {
        if (std::filesystem::is_directory(path)) {
            if (IsCubemapFolder(path)) return icons[IconType::Cubemap];
            return icons[IconType::Folder];
        }

        std::string ext = path.extension().string();
        for (auto& c : ext) c = std::tolower(c);

        if (ext == ".lua") return icons[IconType::Lua];
        if (ext == ".obj" || ext == ".fbx") return icons[IconType::Model];
        if (ext == ".png" || ext == ".jpg" || ext == ".tga") return icons[IconType::Image];
        if (ext == ".glsl" || ext == ".vert" || ext == ".frag") return icons[IconType::Shader];

        return icons[IconType::File];
    }

    bool ContentBrowserPanel::IsCubemapFolder(const std::filesystem::path& dir) {
        if (!std::filesystem::is_directory(dir)) return false;
        // A cubemap folder must contain at least one of the standard face files
        static const char* faceNames[] = { "right", "left", "top", "bottom", "front", "back", "px", "nx", "py", "ny", "pz", "nz" };
        for (auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::string stem = entry.path().stem().string();
                for (auto& c : stem) c = std::tolower(c);
                for (const char* face : faceNames) {
                    if (stem == face) return true;
                }
            }
        }
        return false;
    }

    void ContentBrowserPanel::OnUIRender(EditorContext& context) {
        ImGui::Begin("Content Browser");

        const std::filesystem::path rootPath = Project::GetActiveProject() ? Project::GetActiveProject()->GetAssetDirectory() : "Assets";

        // Breadcrumbs & Navigation
        if (currentDirectory != rootPath) {
            if (ImGui::Button("<- Back")) {
                currentDirectory = currentDirectory.parent_path();
            }
            ImGui::SameLine();
        }
        
        ImGui::Text("Assets/%s", std::filesystem::relative(currentDirectory, rootPath).string().c_str());
        ImGui::Separator();

        // Asset Grid
        float padding = 16.0f;
        float thumbnailSize = 64.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1) columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        if (std::filesystem::exists(currentDirectory)) {
            bool isEmpty = true;
            for (auto& directoryEntry : std::filesystem::directory_iterator(currentDirectory)) {
                isEmpty = false;
                const auto& path = directoryEntry.path();
                std::string filenameString = path.filename().string();

                ImGui::PushID(filenameString.c_str());
                
                RTBEngine::Rendering::Texture* icon = GetIconForFile(path);
                ImTextureID textureID = (ImTextureID)(intptr_t)(icon ? icon->GetID() : 0);

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // Transparent background
                if (ImGui::ImageButton(filenameString.c_str(), textureID, ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 1), ImVec2(1, 0))) {
                    if (directoryEntry.is_directory()) {
                        currentDirectory /= path.filename();
                    }
                }
                ImGui::PopStyleColor();

                // Drag-and-drop source
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    std::string ext = path.extension().string();
                    for (auto& c : ext) c = std::tolower(c);

                    // Check if it's a cubemap folder
                    if (directoryEntry.is_directory() && IsCubemapFolder(path)) {
                        CubemapPayload payload;
                        std::string relativePath = std::filesystem::relative(path, rootPath).string();
                        strncpy_s(payload.path, relativePath.c_str(), sizeof(payload.path) - 1);
                        payload.path[sizeof(payload.path) - 1] = '\0';

                        ImGui::SetDragDropPayload(PAYLOAD_CUBEMAP, &payload, sizeof(CubemapPayload));
                        ImGui::Text("Cubemap: %s", filenameString.c_str());
                    }
                    // Check if it's a texture file
                    else if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
                        ext == ".tga" || ext == ".dds" || ext == ".bmp") {

                        TexturePayload payload;
                        std::string relativePath = std::filesystem::relative(path, rootPath).string();
                        strncpy_s(payload.path, relativePath.c_str(), sizeof(payload.path) - 1);
                        payload.path[sizeof(payload.path) - 1] = '\0';

                        ImGui::SetDragDropPayload(PAYLOAD_TEXTURE, &payload, sizeof(TexturePayload));
                        ImGui::Text("Texture: %s", filenameString.c_str());
                    }
                    // Check if it's a mesh file
                    else if (ext == ".fbx" || ext == ".obj" || ext == ".gltf" || ext == ".glb") {
                        MeshPayload payload;
                        std::string relativePath = std::filesystem::relative(path, rootPath).string();
                        strncpy_s(payload.path, relativePath.c_str(), sizeof(payload.path) - 1);
                        payload.path[sizeof(payload.path) - 1] = '\0';

                        ImGui::SetDragDropPayload(PAYLOAD_MESH, &payload, sizeof(MeshPayload));
                        ImGui::Text("Mesh: %s", filenameString.c_str());
                    }
                    // Check if it's an audio file
                    else if (ext == ".wav" || ext == ".mp3" || ext == ".ogg") {
                        AudioClipPayload payload;
                        std::string relativePath = std::filesystem::relative(path, rootPath).string();
                        strncpy_s(payload.path, relativePath.c_str(), sizeof(payload.path) - 1);
                        payload.path[sizeof(payload.path) - 1] = '\0';

                        ImGui::SetDragDropPayload(PAYLOAD_AUDIOCLIP, &payload, sizeof(AudioClipPayload));
                        ImGui::Text("Audio: %s", filenameString.c_str());
                    }
                    // Check if it's a font file
                    else if (ext == ".ttf" || ext == ".otf") {
                        FontPayload payload;
                        std::string relativePath = std::filesystem::relative(path, rootPath).string();
                        strncpy_s(payload.path, relativePath.c_str(), sizeof(payload.path) - 1);
                        payload.path[sizeof(payload.path) - 1] = '\0';

                        ImGui::SetDragDropPayload(PAYLOAD_FONT, &payload, sizeof(FontPayload));
                        ImGui::Text("Font: %s", filenameString.c_str());
                    }

                    ImGui::EndDragDropSource();
                }


                // Selection logic (visual only for now)
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    if (directoryEntry.is_directory()) {
                        currentDirectory /= path.filename();
                    }
                }

                ImGui::TextWrapped("%s", filenameString.c_str());

                ImGui::NextColumn();
                ImGui::PopID();
            }

            if (isEmpty) {
                ImGui::Columns(1);
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[Empty Directory]");
            }
        }

        ImGui::Columns(1);
        ImGui::End();
    }

}
