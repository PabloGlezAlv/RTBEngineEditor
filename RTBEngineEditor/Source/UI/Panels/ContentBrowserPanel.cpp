#include "ContentBrowserPanel.h"
#include <imgui.h>
#include <RTBEngine/Core/Logger.h>
#include <RTBEngine/Core/ResourceManager.h>
#include <RTBEngine/Scene/Prefab.h>
#include <RTBEngine/Scene/PrefabRegistry.h>
#include <RTBEngine/Scripting/PrefabSaver.h>
#include <RTBEngine/Scene/SceneManager.h>
#include <RTBEngine/Scene/Scene.h>
#include <RTBEngine/Scripting/SceneSaver.h>
#include <RTBEngine/Rendering/ModelLoader.h>
#include <RTBEngine/Rendering/ShaderAsset.h>
#include "../../Project/Project.h"
#include "../DragDropPayloads.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <Windows.h>
#include <shellapi.h>

namespace RTBEditor {

    namespace {
        std::filesystem::path GetAssetRootPath() {
            Project* project = Project::GetActiveProject();
            return project ? project->GetAssetRootPath() : std::filesystem::path("Assets");
        }

        std::string MakeAssetReference(const std::filesystem::path& relativePath) {
            Project* project = Project::GetActiveProject();
            if (project) {
                return project->GetAssetReferencePath(relativePath);
            }
            return (std::filesystem::path("Assets") / relativePath).lexically_normal().generic_string();
        }

        std::string ToLowerCopy(std::string value) {
            for (char& ch : value) {
                ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }
            return value;
        }

        std::vector<std::filesystem::directory_entry> CollectSortedDirectoryEntries(
            const std::filesystem::path& directory)
        {
            std::vector<std::filesystem::directory_entry> entries;
            if (!std::filesystem::exists(directory)) {
                return entries;
            }

            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                entries.push_back(entry);
            }

            std::sort(entries.begin(), entries.end(),
                [](const std::filesystem::directory_entry& left,
                   const std::filesystem::directory_entry& right) {
                    const bool leftIsDirectory = left.is_directory();
                    const bool rightIsDirectory = right.is_directory();
                    if (leftIsDirectory != rightIsDirectory) {
                        return leftIsDirectory;
                    }

                    return ToLowerCopy(left.path().filename().string()) <
                        ToLowerCopy(right.path().filename().string());
                });

            return entries;
        }

        void OpenGameScriptsProjectOrFiles(const std::filesystem::path& primaryFile,
            const std::filesystem::path& secondaryFallback = {}) {
            Project* project = Project::GetActiveProject();
            const std::filesystem::path scriptsProjectPath =
                project ? project->GetGameScriptsProjectPath() : std::filesystem::path();

            if (!scriptsProjectPath.empty() && std::filesystem::exists(scriptsProjectPath)) {
                ShellExecuteA(nullptr, "open", scriptsProjectPath.string().c_str(), nullptr, nullptr, SW_SHOW);
                return;
            }

            RTB_ERROR("GameScripts project not found at: " +
                (scriptsProjectPath.empty() ? std::string("<empty path>") : scriptsProjectPath.string()));

            if (!primaryFile.empty()) {
                ShellExecuteA(nullptr, "open", primaryFile.string().c_str(), nullptr, nullptr, SW_SHOW);
            }
            if (!secondaryFallback.empty()) {
                ShellExecuteA(nullptr, "open", secondaryFallback.string().c_str(), nullptr, nullptr, SW_SHOW);
            }
        }
    }

    ContentBrowserPanel::ContentBrowserPanel() {
        if (Project::GetActiveProject()) {
            currentDirectory = Project::GetActiveProject()->GetAssetRootPath();
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
        icons[IconType::Prefab] = rm.LoadTexture("Default/Icons/prefab.png");

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
        if (ext == ".shader") return icons[IconType::Shader];
        if (ext == ".prefab") return icons[IconType::Prefab];
        if (ext == ".texture") return icons[IconType::Image];
        if (ext == ".rtbasset") return icons[IconType::Lua];

        return icons[IconType::File];
    }

    void ContentBrowserPanel::LoadFbxTextures(const std::filesystem::path& fbxPath)
    {
        expandedFbxPath = fbxPath;
        expandedFbxTextures.clear();

        std::filesystem::path assetRoot = GetAssetRootPath();

        std::string fullPath = fbxPath.string();
        for (char& c : fullPath) if (c == '\\') c = '/';

        RTBEngine::Rendering::ModelData modelData =
            RTBEngine::Rendering::ModelLoader::LoadModelWithAnimations(fullPath);

        if (modelData.embeddedTextures.empty()) return;

        auto& rm = RTBEngine::Core::ResourceManager::GetInstance();
        std::filesystem::path fbxDir = fbxPath.parent_path();
        std::string fbxStem = fbxPath.stem().string();

        // Build a name per embedded texture from material data
        // materials[i].embeddedTextureIndex points into embeddedTextures
        std::vector<std::string> texNames(modelData.embeddedTextures.size());
        for (size_t i = 0; i < modelData.materials.size(); i++) {
            int idx = modelData.materials[i].embeddedTextureIndex;
            if (idx < 0 || idx >= (int)texNames.size()) continue;
            if (!texNames[idx].empty()) continue;
            std::string matName = modelData.materials[i].name;
            // Sanitize name for filesystem
            for (char& c : matName) if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') c = '_';
            texNames[idx] = matName.empty() ? ("tex_" + std::to_string(idx)) : matName;
        }

        for (size_t i = 0; i < modelData.embeddedTextures.size(); i++) {
            const auto& embTex = modelData.embeddedTextures[i];
            std::string name = texNames[i].empty() ? (fbxStem + "_tex" + std::to_string(i)) : (fbxStem + "_" + texNames[i]);

            // Detect actual image format from compressed data header
            std::string ext = ".png";
            if (embTex.isCompressed && embTex.data.size() >= 4) {
                const unsigned char* hdr = embTex.data.data();
                if (hdr[0] == 0xFF && hdr[1] == 0xD8) {
                    ext = ".jpg";
                }
            }

            std::filesystem::path outPath = fbxDir / (name + ext);

            // Check for previously extracted file with wrong extension
            std::filesystem::path altPath = fbxDir / (name + (ext == ".jpg" ? ".png" : ".jpg"));
            if (!std::filesystem::exists(outPath) && std::filesystem::exists(altPath)) {
                outPath = altPath;
            }

            // Write to disk only if it doesn't exist yet
            if (!std::filesystem::exists(outPath)) {
                bool written = false;
                if (embTex.isCompressed) {
                    // Raw compressed bytes (JPG or PNG) — write with matching extension
                    std::ofstream f(outPath, std::ios::binary);
                    if (f.is_open()) {
                        f.write(reinterpret_cast<const char*>(embTex.data.data()), embTex.data.size());
                        written = f.good();
                    }
                } else {
                    // Uncompressed RGBA — encode as PNG via stb_image_write
                    written = stbi_write_png(outPath.string().c_str(),
                        embTex.width, embTex.height, embTex.channels,
                        embTex.data.data(), embTex.width * embTex.channels) != 0;
                }
                if (!written) continue;
            }

            // Create a .texture asset file with flip=false for FBX embedded textures
            std::filesystem::path relativeImagePath = std::filesystem::relative(outPath, assetRoot);
            std::string fullImagePath = MakeAssetReference(relativeImagePath);

            std::filesystem::path textureAssetPath = outPath;
            textureAssetPath.replace_extension(".texture");
            if (!std::filesystem::exists(textureAssetPath)) {
                std::ofstream tf(textureAssetPath);
                if (tf.is_open()) {
                    tf << "image=" << fullImagePath << "\n";
                    tf << "flip=false\n";
                }
            }

            // Load via the .texture asset so flip=false is applied
            std::string textureAssetRelPath = std::filesystem::relative(textureAssetPath, assetRoot).generic_string();
            RTBEngine::Rendering::Texture* tex = rm.LoadTextureAsset(MakeAssetReference(textureAssetRelPath));
            if (!tex) continue;

            FbxEmbeddedTexture entry;
            entry.texture = tex;
            entry.name = name;
            entry.diskPath = textureAssetRelPath;
            expandedFbxTextures.push_back(entry);
        }
    }

    void ContentBrowserPanel::OnUIRender(EditorContext& context) {
        ImGui::Begin("Content Browser");

        const std::filesystem::path rootPath = GetAssetRootPath();

        // Breadcrumbs & Navigation
        if (currentDirectory != rootPath) {
            if (ImGui::Button("<- Back")) {
                currentDirectory = currentDirectory.parent_path();
                selectedPath.clear();
                renamingPath.clear();
                context.selectedAssetPath.clear();
                expandedFbxPath.clear();
                expandedFbxTextures.clear();
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
            const std::vector<std::filesystem::directory_entry> sortedEntries =
                CollectSortedDirectoryEntries(currentDirectory);
            for (const auto& directoryEntry : sortedEntries) {
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
                    if (clickedExt == ".cubemap" || clickedExt == ".texture" || clickedExt == ".shader"
                        || clickedExt == ".h" || clickedExt == ".cpp" || clickedExt == ".vert"
                        || clickedExt == ".frag" || clickedExt == ".glsl" || clickedExt == ".lua"
                        || clickedExt == ".fbx" || clickedExt == ".obj" || clickedExt == ".gltf" || clickedExt == ".glb"
                        || clickedExt == ".rtbasset" || clickedExt == ".prefab") {
                        context.selectedAssetPath = path;
                        context.selectedGameObject = nullptr;
                        // Toggle FBX texture expansion
                        if (clickedExt == ".fbx" || clickedExt == ".obj" || clickedExt == ".gltf" || clickedExt == ".glb") {
                            if (expandedFbxPath == path) {
                                expandedFbxPath.clear();
                                expandedFbxTextures.clear();
                            } else {
                                LoadFbxTextures(path);
                            }
                        }
                    } else {
                        context.selectedAssetPath.clear();
                    }
                }

                // Double click — enter folder or open scene
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    if (directoryEntry.is_directory() && renamingPath != path) {
                        currentDirectory /= path.filename();
                        selectedPath.clear();
                        renamingPath.clear();
                        context.selectedAssetPath.clear();
                        expandedFbxPath.clear();
                        expandedFbxTextures.clear();
                    }
                    else if (!directoryEntry.is_directory()) {
                        std::string ext = path.extension().string();
                        for (auto& c : ext) c = std::tolower(c);
                        if (ext == ".lua") {
                            context.pendingSceneLoad = MakeAssetReference(std::filesystem::relative(path, rootPath));
                        } else if (ext == ".prefab") {
                            context.pendingPrefabOpen = path;
                        }
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
                    else if (ext == ".texture") {
                        TexturePayload payload;
                        std::string relativePath = std::filesystem::relative(path, rootPath).string();
                        strncpy_s(payload.path, relativePath.c_str(), sizeof(payload.path) - 1);
                        payload.path[sizeof(payload.path) - 1] = '\0';

                        ImGui::SetDragDropPayload(PAYLOAD_TEXTURE, &payload, sizeof(TexturePayload));
                        ImGui::Text("Texture: %s", filenameString.c_str());
                    }
                    else if (ext == ".prefab") {
                        PrefabPayload payload;
                        std::string relativePath = std::filesystem::relative(path, rootPath).string();
                        strncpy_s(payload.path, relativePath.c_str(), sizeof(payload.path) - 1);
                        payload.path[sizeof(payload.path) - 1] = '\0';

                        ImGui::SetDragDropPayload(PAYLOAD_PREFAB, &payload, sizeof(PrefabPayload));
                        ImGui::Text("Prefab: %s", filenameString.c_str());
                    }
                    else if (ext == ".lua") {
                        ScenePayload payload;
                        std::string relativePath = std::filesystem::relative(path, rootPath).string();
                        strncpy_s(payload.path, relativePath.c_str(), sizeof(payload.path) - 1);
                        payload.path[sizeof(payload.path) - 1] = '\0';

                        ImGui::SetDragDropPayload(PAYLOAD_SCENE, &payload, sizeof(ScenePayload));
                        ImGui::Text("Scene: %s", filenameString.c_str());
                    }


                    ImGui::EndDragDropSource();
                }

                // Drop target: accept GameObject on any item to save as Prefab
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* dropPayload = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT)) {
                        const GameObjectPayload* data = static_cast<const GameObjectPayload*>(dropPayload->Data);
                        RTBEngine::ECS::GameObject* go = reinterpret_cast<RTBEngine::ECS::GameObject*>(data->gameObjectId);

                        std::string prefabName = go->GetName();
                        std::filesystem::path savePath = currentDirectory / (prefabName + ".prefab");

                        auto prefab = RTBEngine::ECS::Prefab::CreateFromGameObject(go);
                        if (prefab) {
                            RTBEngine::Scripting::PrefabSaver::Save(*prefab, savePath.string());
                            RTBEngine::ECS::PrefabRegistry::GetInstance().Register(savePath.string());

                            go->SetPrefabName(prefabName);
                            RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();
                        }
                    }
                    ImGui::EndDragDropTarget();
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
                                // When renaming a .lua scene that is currently loaded, update its internal name
                                {
                                    std::string ext = path.extension().string();
                                    for (auto& c : ext) c = std::tolower(c);
                                    if (ext == ".lua") {
                                        auto& sm = RTBEngine::ECS::SceneManager::GetInstance();
                                        auto& resources = RTBEngine::Core::ResourceManager::GetInstance();
                                        namespace fs = std::filesystem;
                                        fs::path assetRoot = GetAssetRootPath();
                                        fs::path oldAbsolute = fs::path(resources.ResolvePathForRead(path.string())).lexically_normal();
                                        fs::path activeAbsolute = fs::path(resources.ResolvePathForRead(sm.GetActiveScenePath())).lexically_normal();
                                        if (oldAbsolute == activeAbsolute) {
                                            RTBEngine::ECS::Scene* scene = sm.GetActiveScene();
                                            if (scene) {
                                                fs::path newRelative = fs::relative(newPath, assetRoot);
                                                std::string newScenePath = MakeAssetReference(newRelative);
                                                scene->SetName(newName);
                                                sm.SetActiveScenePath(newScenePath);
                                                if (RTBEngine::Scripting::SceneSaver::SaveScene(scene, newScenePath)) {
                                                    sm.ClearSceneDirty();
                                                }
                                            }
                                        }
                                    }
                                }
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
                    // Show expand arrow indicator for FBX files that have embedded textures
                    std::string fileExt = path.extension().string();
                    for (auto& c : fileExt) c = std::tolower(c);
                    bool isMeshFile = (fileExt == ".fbx" || fileExt == ".obj" || fileExt == ".gltf" || fileExt == ".glb");
                    bool isExpanded = (expandedFbxPath == path && !expandedFbxTextures.empty());
                    if (isMeshFile) {
                        ImGui::TextWrapped("%s %s", isExpanded ? "v" : ">", filenameString.c_str());
                    } else {
                        ImGui::TextWrapped("%s", filenameString.c_str());
                    }
                }

                ImGui::NextColumn();
                ImGui::PopID();

                // Draw embedded texture sub-items right after the expanded FBX
                if (expandedFbxPath == path && !expandedFbxTextures.empty()) {
                    for (size_t ti = 0; ti < expandedFbxTextures.size(); ti++) {
                        const auto& entry = expandedFbxTextures[ti];
                        std::string subId = "##fbxtex_" + std::to_string(ti);

                        ImGui::PushID(subId.c_str());

                        // Slightly tinted background to visually group them under the FBX
                        ImTextureID subTexID = (ImTextureID)(intptr_t)entry.texture->GetID();
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.25f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.35f, 0.6f, 1.0f));
                        ImGui::ImageButton(subId.c_str(), subTexID,
                            ImVec2(thumbnailSize, thumbnailSize), ImVec2(0, 1), ImVec2(1, 0));
                        ImGui::PopStyleColor(2);

                        // Drag source — standard TexturePayload with real disk path
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                            TexturePayload payload;
                            strncpy_s(payload.path, entry.diskPath.c_str(), sizeof(payload.path) - 1);
                            payload.path[sizeof(payload.path) - 1] = '\0';
                            ImGui::SetDragDropPayload(PAYLOAD_TEXTURE, &payload, sizeof(TexturePayload));
                            ImGui::Image(subTexID, ImVec2(32.0f, 32.0f), ImVec2(0, 1), ImVec2(1, 0));
                            ImGui::SameLine();
                            ImGui::Text("%s", entry.name.c_str());
                            ImGui::EndDragDropSource();
                        }

                        ImGui::TextWrapped("%s", entry.name.c_str());
                        ImGui::NextColumn();
                        ImGui::PopID();
                    }
                }
            }

            if (isEmpty) {
                ImGui::Columns(1);
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[Empty Directory]");
            }
        }
        // Drop target: receive GameObject from Hierarchy to save as Prefab
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT)) {
                const GameObjectPayload* data = static_cast<const GameObjectPayload*>(payload->Data);
                RTBEngine::ECS::GameObject* go = reinterpret_cast<RTBEngine::ECS::GameObject*>(data->gameObjectId);

                std::string prefabName = go->GetName();
                std::filesystem::path savePath = currentDirectory / (prefabName + ".prefab");

                auto prefab = RTBEngine::ECS::Prefab::CreateFromGameObject(go);
                if (prefab) {
                    RTBEngine::Scripting::PrefabSaver::Save(*prefab, savePath.string());
                    RTBEngine::ECS::PrefabRegistry::GetInstance().Register(savePath.string());
                    
                    //Used gameobject is now an instance of a prefab
                    go->SetPrefabName(prefabName);
                    RTBEngine::ECS::SceneManager::GetInstance().MarkSceneDirty();

                }
            }
            ImGui::EndDragDropTarget();
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
                        h << "#include <RTBEngine/Scene/Component.h>\n";
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

                    OpenGameScriptsProjectOrFiles(headerFile, cppFile);

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

                    OpenGameScriptsProjectOrFiles(headerFile, cppFile);

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

                if (ImGui::MenuItem("Shader Asset")) {
                    std::filesystem::path shadersDirectory = currentDirectory / "Shaders";
                    if (!std::filesystem::exists(shadersDirectory)) {
                        shadersDirectory = currentDirectory;
                    }

                    std::filesystem::path newFile = shadersDirectory / "NewShader.shader";
                    int suffix = 1;
                    while (std::filesystem::exists(newFile)) {
                        newFile = shadersDirectory / ("NewShader" + std::to_string(suffix++) + ".shader");
                    }

                    Project* project = Project::GetActiveProject();
                    const std::filesystem::path assetRoot =
                        project ? project->GetAssetRootPath() : GetAssetRootPath();
                    if (RTBEngine::Rendering::ShaderAsset::CreateTemplate(newFile, assetRoot)) {
                        RTBEngine::Core::ResourceManager::GetInstance().ScanShaderAssets(assetRoot);
                        selectedPath = newFile;
                        renamingPath = newFile;
                        strncpy_s(renameBuffer, newFile.stem().string().c_str(), sizeof(renameBuffer) - 1);
                    }
                }

                if (ImGui::MenuItem("Texture Asset")) {
                    std::filesystem::path newFile = currentDirectory / "NewTexture.texture";
                    int suffix = 1;
                    while (std::filesystem::exists(newFile)) {
                        newFile = currentDirectory / ("NewTexture" + std::to_string(suffix++) + ".texture");
                    }
                    std::ofstream f(newFile);
                    f << "image=\n";
                    f << "flip=true\n";
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
