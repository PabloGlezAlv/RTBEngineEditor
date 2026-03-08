#include "ContentBrowserPanel.h"
#include <imgui.h>
#include <RTBEngine/Core/ResourceManager.h>
#include "../../Project/Project.h"
#include "../DragDropPayloads.h"
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <shellapi.h>

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
            return icons[IconType::Folder];
        }

        std::string ext = path.extension().string();
        for (auto& c : ext) c = std::tolower(c);

        if (ext == ".cubemap") return icons[IconType::Cubemap];
        if (ext == ".lua") return icons[IconType::Lua];
        if (ext == ".obj" || ext == ".fbx") return icons[IconType::Model];
        if (ext == ".png" || ext == ".jpg" || ext == ".tga") return icons[IconType::Image];
        if (ext == ".glsl" || ext == ".vert" || ext == ".frag") return icons[IconType::Shader];

        return icons[IconType::File];
    }

    void ContentBrowserPanel::OnUIRender(EditorContext& context) {
        ImGui::Begin("Content Browser");

        const std::filesystem::path rootPath = Project::GetActiveProject() ? Project::GetActiveProject()->GetAssetDirectory() : "Assets";

        // Breadcrumbs & Navigation
        if (currentDirectory != rootPath) {
            if (ImGui::Button("<- Back")) {
                currentDirectory = currentDirectory.parent_path();
                selectedPath.clear();
                renamingPath.clear();
                context.selectedAssetPath.clear();
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

                bool isSelected = (selectedPath == path);
                ImVec4 tint = isSelected ? ImVec4(0.3f, 0.5f, 0.9f, 0.4f) : ImVec4(0, 0, 0, 0);
                ImGui::PushStyleColor(ImGuiCol_Button, tint);
                ImGui::ImageButton(filenameString.c_str(), textureID, ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::PopStyleColor();

                // Single click — select
                if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                    selectedPath = path;
                    // Expose asset files to the Inspector via context
                    std::string clickedExt = path.extension().string();
                    for (auto& c : clickedExt) c = std::tolower(c);
                    if (clickedExt == ".cubemap" || clickedExt == ".h" || clickedExt == ".cpp") {
                        context.selectedAssetPath = path;
                        context.selectedGameObject = nullptr;
                    } else {
                        context.selectedAssetPath.clear();
                    }
                }

                // Double click — enter folder
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    if (directoryEntry.is_directory() && renamingPath != path) {
                        currentDirectory /= path.filename();
                        selectedPath.clear();
                        renamingPath.clear();
                        context.selectedAssetPath.clear();
                    }
                }
                // Double click — enter folder
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    if (directoryEntry.is_directory() && renamingPath != path) {
                        currentDirectory /= path.filename();
                        selectedPath.clear();
                        renamingPath.clear();
                        context.selectedAssetPath.clear();
                    }
                    else if (!directoryEntry.is_directory()) {
                        std::string ext = path.extension().string();
                        for (auto& c : ext) c = std::tolower(c);
                        if (ext == ".lua") {
                            context.pendingSceneLoad = path;
                        }
                }


                // F2 to rename selected item
                if (isSelected && ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_F2) && renamingPath != path) {
                    renamingPath = path;
                    strncpy_s(renameBuffer, path.stem().string().c_str(), sizeof(renameBuffer) - 1);
                }

                // Drag-and-drop source
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    std::string ext = path.extension().string();
                    for (auto& c : ext) c = std::tolower(c);

                    // Check if it's a .cubemap asset file
                    if (ext == ".cubemap") {
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


                // Inline rename
                if (renamingPath == path) {
                    ImGui::SetNextItemWidth(thumbnailSize + padding - 4.0f);
                    ImGui::SetKeyboardFocusHere();
                    if (ImGui::InputText("##rename", renameBuffer, sizeof(renameBuffer),
                        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {

                        std::string newName = renameBuffer;
                        if (!newName.empty() && newName != path.stem().string()) {
                            std::filesystem::path newPath = path.parent_path() / (newName + path.extension().string());
                            if (!std::filesystem::exists(newPath)) {
                                std::filesystem::rename(path, newPath);
                                // When renaming a .h, also rename the companion .cpp and update the .vcxproj
                                if (path.extension() == ".h") {
                                    std::filesystem::path oldCpp = path.parent_path() / (path.stem().string() + ".cpp");
                                    std::filesystem::path newCpp = path.parent_path() / (newName + ".cpp");
                                    if (std::filesystem::exists(oldCpp) && !std::filesystem::exists(newCpp)) {
                                        std::filesystem::rename(oldCpp, newCpp);
                                    }

                                    // Patch GameScripts.vcxproj with the new filenames
                                    // No need to patch GameScripts.vcxproj: it uses wildcard includes (Assets\\**\\*.cpp).
                                }
                                selectedPath = newPath;
                            }
                        }
                        renamingPath.clear();
                    }
                    // Cancel on Escape or focus lost
                    if (ImGui::IsKeyPressed(ImGuiKey_Escape) || (!ImGui::IsItemActive() && !ImGui::IsItemFocused())) {
                        renamingPath.clear();
                    }
                } else {
                    ImGui::TextWrapped("%s", filenameString.c_str());
                }

                ImGui::NextColumn();
                ImGui::PopID();
            }

            if (isEmpty) {
                ImGui::Columns(1);
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[Empty Directory]");
            }
        }

        // Click on empty area deselects
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
            selectedPath.clear();
            renamingPath.clear();
            context.selectedAssetPath.clear();
        }

        // Delete selected item with Supr
        if (!selectedPath.empty() && ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
            std::filesystem::remove_all(selectedPath);
            selectedPath.clear();
            renamingPath.clear();
            context.selectedAssetPath.clear();
        }

        DrawContextMenu();

        ImGui::Columns(1);
        ImGui::End();
    }

    void ContentBrowserPanel::DrawContextMenu() {
        if (ImGui::BeginPopupContextWindow("ContentBrowserContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {

            if (ImGui::BeginMenu("Create")) {

                if (ImGui::MenuItem("Folder")) {
                    std::filesystem::path newFolder = currentDirectory / "New Folder";
                    int suffix = 1;
                    while (std::filesystem::exists(newFolder)) {
                        newFolder = currentDirectory / ("New Folder " + std::to_string(suffix++));
                    }
                    std::filesystem::create_directory(newFolder);
                    selectedPath = newFolder;
                    renamingPath = newFolder;
                    strncpy_s(renameBuffer, newFolder.stem().string().c_str(), sizeof(renameBuffer) - 1);
                }

                ImGui::Separator();

                if (ImGui::MenuItem("C++ Component")) {
                    // Scripts live in the current browser directory (inside Assets/)
                    // and compile into GameScripts.dll via GameScripts.vcxproj
                    std::filesystem::path scriptsDir = currentDirectory;
                    std::filesystem::create_directories(scriptsDir);

                    std::filesystem::path headerFile = scriptsDir / "NewComponent.h";
                    std::filesystem::path cppFile    = scriptsDir / "NewComponent.cpp";
                    int suffix = 1;
                    while (std::filesystem::exists(headerFile) || std::filesystem::exists(cppFile)) {
                        std::string name = "NewComponent" + std::to_string(suffix++);
                        headerFile = scriptsDir / (name + ".h");
                        cppFile    = scriptsDir / (name + ".cpp");
                    }
                    std::string className = headerFile.stem().string();

                    // Write .h
                    {
                        std::ofstream h(headerFile);
                        h << "#pragma once\n";
                        h << "#include <RTBEngine/ECS/Component.h>\n";
                        h << "#include <RTBEngine/Reflection/PropertyMacros.h>\n";
                        h << "\n";
                        h << "class " << className << " : public RTBEngine::ECS::Component {\n";
                        h << "public:\n";
                        h << "    " << className << "();\n";
                        h << "    ~" << className << "() override;\n";
                        h << "\n";
                        h << "    " << className << "(const " << className << "&) = delete;\n";
                        h << "    " << className << "& operator=(const " << className << "&) = delete;\n";
                        h << "\n";
                        h << "    //Loop methods\n";
                        h << "    void OnAwake() override;\n";
                        h << "    void OnStart() override;\n";
                        h << "    void OnUpdate(float deltaTime) override;\n";
                        h << "    void OnFixedUpdate(float fixedDeltaTime) override;\n";
                        h << "    void OnDestroy() override;\n";
                        h << "\n";
                        h << "    // Reflected properties (Proxy)\n";
                        h << "    float speedRef = 1.0f;\n";
                        h << "\n";
                        h << "    RTB_COMPONENT(" << className << ")\n";
                        h << "\n";
                        h << "private:\n";
                        h << "    float speed = 1.0f;\n";
                        h << "};\n";
                    }

                    // Write .cpp
                    {
                        std::ofstream cpp(cppFile);
                        cpp << "#include \"" << className << ".h\"\n";
                        cpp << "\n";
                        cpp << "using ThisClass = " << className << ";\n";
                        cpp << "RTB_REGISTER_COMPONENT(" << className << ")\n";
                        cpp << "RTB_END_REGISTER(" << className << ")\n";
                        cpp << "\n";
                        cpp << className << "::" << className << "() {}\n";
                        cpp << className << "::~" << className << "() {}\n";
                        cpp << "\n";
                        cpp << "void " << className << "::OnAwake() {}\n";
                        cpp << "\n";
                        cpp << "void " << className << "::OnStart() {}\n";
                        cpp << "\n";
                        cpp << "void " << className << "::OnUpdate(float deltaTime) {}\n";
                        cpp << "\n";
                        cpp << "void " << className << "::OnFixedUpdate(float fixedDeltaTime) {}\n";
                        cpp << "\n";
                        cpp << "void " << className << "::OnDestroy() {}\n";
                    }

                    // No need to register files explicitly in GameScripts.vcxproj:
                    // it compiles all C++ files under Assets automatically.

                    // Open the C++ project/solution instead of just the files so that
                    // includes, IntelliSense and build settings are all correctly configured.
                    // Prefer the RTBEngineEditor solution, which already contains GameScripts.vcxproj.
                    {
                        std::filesystem::path solutionPath =
                            std::filesystem::current_path() / "RTBEngineEditor.sln";

                        if (std::filesystem::exists(solutionPath)) {
                            ShellExecuteA(
                                nullptr,
                                "open",
                                solutionPath.string().c_str(),
                                nullptr,
                                nullptr,
                                SW_SHOW
                            );
                        }
                        else {
                            // Fallback: open the individual files with the default editor.
                            ShellExecuteA(nullptr, "open", headerFile.string().c_str(), nullptr, nullptr, SW_SHOW);
                            ShellExecuteA(nullptr, "open", cppFile.string().c_str(), nullptr, nullptr, SW_SHOW);
                        }
                    }

                    selectedPath = headerFile;
                    renamingPath = headerFile;
                    strncpy_s(renameBuffer, className.c_str(), sizeof(renameBuffer) - 1);
                }

                if (ImGui::MenuItem("C++ Class")) {
                    std::filesystem::path scriptsDir = currentDirectory;
                    std::filesystem::create_directories(scriptsDir);

                    std::filesystem::path headerFile = scriptsDir / "NewClass.h";
                    std::filesystem::path cppFile    = scriptsDir / "NewClass.cpp";
                    int suffix = 1;
                    while (std::filesystem::exists(headerFile) || std::filesystem::exists(cppFile)) {
                        std::string name = "NewClass" + std::to_string(suffix++);
                        headerFile = scriptsDir / (name + ".h");
                        cppFile    = scriptsDir / (name + ".cpp");
                    }
                    std::string className = headerFile.stem().string();

                    // Write .h
                    {
                        std::ofstream h(headerFile);
                        h << "#pragma once\n";
                        h << "\n";
                        h << "class " << className << " {\n";
                        h << "public:\n";
                        h << "    " << className << "();\n";
                        h << "    ~" << className << "();\n";
                        h << "\n";
                        h << "    " << className << "(const " << className << "&) = delete;\n";
                        h << "    " << className << "& operator=(const " << className << "&) = delete;\n";
                        h << "};\n";
                    }

                    // Write .cpp
                    {
                        std::ofstream cpp(cppFile);
                        cpp << "#include \"" << className << ".h\"\n";
                        cpp << "\n";
                        cpp << className << "::" << className << "() {}\n";
                        cpp << className << "::~" << className << "() {}\n";
                    }

                    {
                        std::filesystem::path solutionPath =
                            std::filesystem::current_path() / "RTBEngineEditor.sln";

                        if (std::filesystem::exists(solutionPath)) {
                            ShellExecuteA(nullptr, "open", solutionPath.string().c_str(), nullptr, nullptr, SW_SHOW);
                        } else {
                            ShellExecuteA(nullptr, "open", headerFile.string().c_str(), nullptr, nullptr, SW_SHOW);
                            ShellExecuteA(nullptr, "open", cppFile.string().c_str(), nullptr, nullptr, SW_SHOW);
                        }
                    }

                    selectedPath = headerFile;
                    renamingPath = headerFile;
                    strncpy_s(renameBuffer, className.c_str(), sizeof(renameBuffer) - 1);
                }

                if (ImGui::MenuItem("Scene")) {
                    std::filesystem::path newFile = currentDirectory / "NewScene.lua";
                    int suffix = 1;
                    while (std::filesystem::exists(newFile)) {
                        newFile = currentDirectory / ("NewScene" + std::to_string(suffix++) + ".lua");
                    }
                    std::ofstream f(newFile);
                    f << "function CreateScene()\n";
                    f << "    return {\n";
                    f << "        name = \"" << newFile.stem().string() << "\",\n";
                    f << "        skyboxEnabled = true,\n";
                    f << "        gameObjects = {\n";
                    f << "            {\n";
                    f << "                name = \"Main Camera\",\n";
                    f << "                position = Vector3(0.00, 1.00, -5.00),\n";
                    f << "                components = {\n";
                    f << "                    { type = \"CameraComponent\", isMain = true, fov = 60.00, nearPlane = 0.10, farPlane = 1000.00 },\n";
                    f << "                    { type = \"FreeLookCamera\", moveSpeed = 5.00, lookSpeed = 0.10 },\n";
                    f << "                }\n";
                    f << "            },\n";
                    f << "            {\n";
                    f << "                name = \"Directional Light\",\n";
                    f << "                rotation = Quaternion.FromEulerAngles(45.00, -30.00, 0.00),\n";
                    f << "                components = {\n";
                    f << "                    { type = \"LightComponent\", lightType = 0, color = Color(1.00, 1.00, 1.00, 1.00), intensity = 1.00, castShadows = true },\n";
                    f << "                }\n";
                    f << "            },\n";
                    f << "        }\n";
                    f << "    }\n";
                    f << "end\n";
                    selectedPath = newFile;
                    renamingPath = newFile;
                    strncpy_s(renameBuffer, newFile.stem().string().c_str(), sizeof(renameBuffer) - 1);
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Cubemap Asset")) {
                    std::filesystem::path newFile = currentDirectory / "NewCubemap.cubemap";
                    int suffix = 1;
                    while (std::filesystem::exists(newFile)) {
                        newFile = currentDirectory / ("NewCubemap" + std::to_string(suffix++) + ".cubemap");
                    }
                    std::ofstream f(newFile);
                    f << "right=\n";
                    f << "left=\n";
                    f << "top=\n";
                    f << "bottom=\n";
                    f << "front=\n";
                    f << "back=\n";
                    selectedPath = newFile;
                    renamingPath = newFile;
                    strncpy_s(renameBuffer, newFile.stem().string().c_str(), sizeof(renameBuffer) - 1);
                }

                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Show in Explorer")) {
                std::string cmd = "explorer \"" + currentDirectory.string() + "\"";
                system(cmd.c_str());
            }

            ImGui::EndPopup();
        }
    }

}
