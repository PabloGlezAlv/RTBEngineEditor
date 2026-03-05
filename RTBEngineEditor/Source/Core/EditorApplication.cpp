#include "EditorApplication.h"
#include <imgui.h>
#include <GL/glew.h>
#include <iostream>
#include <filesystem>
#include <RTBEngine/ECS/SceneManager.h>
#include <RTBEngine/Rendering/FrameBuffer.h>
#include <RTBEngine/Scripting/SceneSaver.h>
#include <RTBEngine/Scripting/ScriptManager.h>
#include "../UI/Panels/SceneViewPanel.h"
#include "../Build/BuildSystem.h"

namespace RTBEditor {

    EditorApplication::EditorApplication() {}

    EditorApplication::~EditorApplication() {
        Shutdown();
    }

    bool EditorApplication::Initialize(const RTBEngine::Core::ApplicationConfig& config) {
        engineApp = std::make_unique<RTBEngine::Core::Application>(config);
        
        if (!engineApp->Initialize()) {
            return false;
        }

        engineApp->SetIsRunning(true);

        // Load Project Settings
        project = std::make_unique<Project>();
        if (project->Load("MyProject.rtbproj")) {
            RTBEngine::ECS::SceneManager::GetInstance().LoadScene(project->GetStartScene());
        }

        // Initialize UI Layer
        uiLayer = std::make_unique<EditorLayer>();
        uiLayer->Initialize(engineApp->GetWindow()->GetSDLWindow(), engineApp->GetImGuiContext());

        uiLayer->SetupToolbar(
            [this]() { OnPlay(); },
            [this]() { OnPause(); },
            [this]() { OnStop(); },
            [this]() { return GetState(); },
            [this]() { OnCompileScripts(); }
        );

        // Set up menu bar callbacks
        uiLayer->GetMenuBar()->SetBuildCallback([this]() {
            uiLayer->OpenBuildDialog();
        });

        uiLayer->GetMenuBar()->SetExitCallback([this]() {
            isRunning = false;
        });

        uiLayer->GetMenuBar()->SetSaveSceneCallback([this]() {
            auto& sm = RTBEngine::ECS::SceneManager::GetInstance();
            RTBEngine::Scripting::SceneSaver::SaveScene(sm.GetActiveScene(), sm.GetActiveScenePath());
            sm.ClearSceneDirty();
            uiLayer->GetMenuBar()->SetSceneDirty(false);
        });

        // Load GameScripts.dll if it already exists from a previous compilation.
        // Prefer the editor binary folder (x64/Debug) where all engine DLLs live,
        // so the script DLL can resolve its dependencies.
        {
            namespace fs = std::filesystem;
            fs::path projectRoot = fs::current_path();
            fs::path binDirDebug = projectRoot / "x64" / "Debug";
            fs::path binDllPath = binDirDebug / "GameScripts.dll";
            fs::path legacyPath = projectRoot / "GameScripts.dll";

            if (fs::exists(binDllPath)) {
                RTBEngine::Scripting::ScriptManager::GetInstance().LoadScripts(binDllPath.string());
            }
            else if (fs::exists(legacyPath)) {
                RTBEngine::Scripting::ScriptManager::GetInstance().LoadScripts(legacyPath.string());
            }
        }

        isRunning = true;
        return true;
    }

    void EditorApplication::Run() {
        float lastTime = (float)SDL_GetTicks() / 1000.0f;

        while (isRunning && engineApp->IsRunning()) {
            float currentTime = (float)SDL_GetTicks() / 1000.0f;
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            RTBEngine::Input::InputManager::GetInstance().Update();
            engineApp->ProcessInput();
            if (engineApp->GetWindow()->GetShouldClose()) {
                isRunning = false;
            }

            Update(deltaTime);
            Render();
        }
    }

    void EditorApplication::Update(float deltaTime) {
        // Sync dirty flag from SceneManager to menu bar
        uiLayer->GetMenuBar()->SetSceneDirty(
            RTBEngine::ECS::SceneManager::GetInstance().IsSceneDirty()
        );

        if (state == EditorState::Play) {
            engineApp->Update(deltaTime);
        } else {
            // In Edit/Pause mode, sync component properties so inspector changes reflect in the viewport
            RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
            if (scene) {
                for (auto& go : scene->GetGameObjects()) {
                    for (auto& comp : go->GetComponents()) {
                        comp->OnValidate();
                    }
                }
            }
        }
    }

    void EditorApplication::OnPlay() {
        state = EditorState::Play;
        // Focus the Game window when playing
        ImGui::SetWindowFocus("Game");
    }

    void EditorApplication::OnPause() {
        state = EditorState::Pause;
    }

    void EditorApplication::OnStop() {
        state = EditorState::Edit;
        
        if (project) {
            RTBEngine::ECS::SceneManager::GetInstance().LoadScene(project->GetStartScene());
        } else {
            // Fallback
             RTBEngine::ECS::SceneManager::GetInstance().LoadScene("Default/Scenes/DefaultScene.lua");
        }
    }

    void EditorApplication::Render() {
        // Render scene to Scene View Panel framebuffer
        RenderSceneToFramebuffer();

        // Clear the main window
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render editor UI
        if (uiLayer) {
            uiLayer->SetEditorState(state);
            uiLayer->Begin();
            uiLayer->OnUIRender();
            uiLayer->End();
        }

        engineApp->GetWindow()->SwapBuffers();
    }

    void EditorApplication::RenderSceneToFramebuffer() {
        if (!uiLayer) return;

        RTBEngine::ECS::Scene* scene = RTBEngine::ECS::SceneManager::GetInstance().GetActiveScene();
        if (!scene) return;

        // 1. Render Scene View (Editor Camera)
        SceneViewPanel* sceneView = uiLayer->GetSceneViewPanel();
        if (sceneView) {
            RTBEngine::Rendering::Framebuffer* framebuffer = sceneView->GetFramebuffer();
            RTBEngine::Rendering::Camera* editorCamera = sceneView->GetEditorCamera();
            int vpWidth = sceneView->GetViewportWidth();
            int vpHeight = sceneView->GetViewportHeight();

            if (framebuffer && editorCamera && vpWidth > 0 && vpHeight > 0) {
                framebuffer->Bind();
                glViewport(0, 0, vpWidth, vpHeight);
                engineApp->RenderShadowPass(scene);
                framebuffer->Bind();
                glViewport(0, 0, vpWidth, vpHeight);
                engineApp->RenderGeometryPass(scene, editorCamera);

                // Render editor grid and axes
                if (sceneView->GetGridRenderer()) {
                    sceneView->GetGridRenderer()->Render(editorCamera);
                }

                framebuffer->Unbind();
            }
        }

        // 2. Render Game View (Main Camera)
        GameViewPanel* gameView = uiLayer->GetGameViewPanel();
        if (gameView) {
            RTBEngine::Rendering::Framebuffer* framebuffer = gameView->GetFramebuffer();
            RTBEngine::ECS::CameraComponent* mainCamComp = scene->GetMainCamera();
            int vpWidth = gameView->GetViewportWidth();
            int vpHeight = gameView->GetViewportHeight();

            // We only need to render Game view if it exists and has a camera
            if (framebuffer && mainCamComp && vpWidth > 0 && vpHeight > 0) {
                RTBEngine::Rendering::Camera* mainCamera = mainCamComp->GetCamera();
                if (mainCamera) {
                    framebuffer->Bind();
                    glViewport(0, 0, vpWidth, vpHeight);
                    // For now, reuse shadow maps from first pass
                    engineApp->RenderGeometryPass(scene, mainCamera);

                    // Note: Scene UI is rendered in GameViewPanel::OnUIRender()
                    // after the framebuffer image, within the ImGui frame

                    framebuffer->Unbind();
                }
            }
        }

        // Restore main viewport
        glViewport(0, 0, engineApp->GetWindow()->GetWidth(), engineApp->GetWindow()->GetHeight());
    }

    void EditorApplication::OnCompileScripts()
    {
        // Path to GameScripts.vcxproj — relative to the editor working directory
        std::string vcxprojPath = (std::filesystem::current_path() / "GameScripts" / "GameScripts.vcxproj").string();

        auto& scriptManager = RTBEngine::Scripting::ScriptManager::GetInstance();

        // Unload current DLL before recompiling so MSBuild can overwrite the file
        scriptManager.UnloadScripts();

        ScriptCompileResult result = BuildSystem::CompileScripts(vcxprojPath);

        if (result == ScriptCompileResult::Success) {
            // MSBuild outputs GameScripts.dll under the project folder (GameScripts/x64/Debug/GameScripts.dll).
            // Copy it to the editor's binary folder (x64/Debug) so it sits next to RTBEngine.dll
            // and other dependencies, then load it from there.
            namespace fs = std::filesystem;

            fs::path projectRoot = fs::current_path();
            fs::path buildDllPath = projectRoot / "GameScripts" / "x64" / "Debug" / "GameScripts.dll";
            fs::path binDirDebug = projectRoot / "x64" / "Debug";
            fs::path targetDllPath = binDirDebug / "GameScripts.dll";

            try {
                if (fs::exists(buildDllPath)) {
                    if (!fs::exists(binDirDebug)) {
                        fs::create_directories(binDirDebug);
                    }
                    fs::copy_file(buildDllPath, targetDllPath, fs::copy_options::overwrite_existing);
                }
            }
            catch (const std::exception& e) {
                RTBEngine::Core::Logger::GetInstance().Error(
                    std::string("EditorApplication::OnCompileScripts - Failed to copy GameScripts.dll: ") + e.what());
            }

            // Reload the freshly compiled DLL — static initializers re-register all components.
            scriptManager.LoadScripts(targetDllPath.string());
        }
    }

    void EditorApplication::Shutdown() {
        // Unload GameScripts.dll before destroying the engine to avoid dangling pointers
        RTBEngine::Scripting::ScriptManager::GetInstance().UnloadScripts();

        if (engineApp) {
            engineApp->Shutdown();
            engineApp.reset();
        }
        isRunning = false;
    }
}
