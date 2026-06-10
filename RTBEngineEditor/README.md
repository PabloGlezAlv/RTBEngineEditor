# RTBEngineEditor

A visual game editor for **RTBEngine**, built in **C++17** using **ImGui** for the entire UI with a docking-based multi-panel layout. The editor consumes the engine through `RTBEngine_SDK` (`RTBEngine.lib` plus `RTBEngine.dll`) and adds an additional layer of tooling on top: scene authoring, property inspection, asset browsing, real-time play testing, and game build export.

---

## Table of Contents

1. [Project Structure](#1-project-structure)
2. [Architecture Overview](#2-architecture-overview)
3. [Editor State Machine](#3-editor-state-machine)
4. [EditorContext](#4-editorcontext)
5. [EditorApplication](#5-editorapplication)
6. [EditorLayer — Docking System](#6-editorlayer--docking-system)
7. [MainMenuBar](#7-mainmenubar)
8. [DragDropPayloads](#8-dragdroppayloads)
9. [Scene Hierarchy Panel](#9-scene-hierarchy-panel)
10. [Inspector Panel](#10-inspector-panel)
11. [Scene View Panel](#11-scene-view-panel)
12. [Game View Panel](#12-game-view-panel)
13. [Content Browser Panel](#13-content-browser-panel)
14. [Console Panel](#14-console-panel)
15. [Toolbar Panel](#15-toolbar-panel)
16. [Build Dialog Modal](#16-build-dialog-modal)
17. [Asset Browser Modal](#17-asset-browser-modal)
18. [Build System](#18-build-system)
19. [Script Components](#19-script-components)
20. [Editor Rendering Utilities](#20-editor-rendering-utilities)
21. [Raycast Utilities](#21-raycast-utilities)
22. [Project System](#22-project-system)
23. [Keyboard Shortcuts](#23-keyboard-shortcuts)
24. [Workflow Guides](#24-workflow-guides)
25. [Online and Multiplayer](#25-online-and-multiplayer)

---

## 1. Project Structure

```
RTBEngineEditor/
├── RTBEngineEditor.sln

└── RTBEngineEditor/
    ├── Source/
    │   ├── Core/
    │   │   ├── EditorApplication.h / .cpp    Main editor loop and state management
    │   │   ├── EditorTypes.h                 EditorState enum, EditorContext struct
    │   │   └── Project.h / .cpp              Project config load/save
    │   │
    │   ├── UI/
    │   │   ├── EditorLayer.h / .cpp          ImGui docking host, panel container
    │   │   ├── MainMenuBar.h / .cpp          File / Window menu bar
    │   │   ├── DragDropPayloads.h            Payload type constants and structs
    │   │   │
    │   │   ├── Panels/
    │   │   │   ├── EditorPanel.h             Base class for all panels
    │   │   │   ├── SceneHierarchyPanel.h / .cpp
    │   │   │   ├── InspectorPanel.h / .cpp
    │   │   │   ├── SceneViewPanel.h / .cpp
    │   │   │   ├── GameViewPanel.h / .cpp
    │   │   │   ├── ContentBrowserPanel.h / .cpp
    │   │   │   ├── ConsolePanel.h / .cpp
    │   │   │   └── ToolbarPanel.h / .cpp
    │   │   │
    │   │   └── Modals/
    │   │       ├── BuildDialog.h / .cpp
    │   │       └── AssetBrowserModal.h / .cpp
    │   │
    │   ├── Build/
    │   │   └── BuildSystem.h / .cpp
    │   │
    │   ├── Rendering/
    │   │   ├── EditorGridRenderer.h / .cpp
    │   │   └── ColliderRenderer.h / .cpp
    │   │
    │   ├── Utils/
    │   │   └── RaycastUtils.h / .cpp
    │   │
    │   └── Components/
    │       └── EditorComponents.h / .cpp     Test/utility script component registration
    │
    ├── Assets/
    │   ├── Cubemap/
    │   ├── Models/
    │   ├── Scenes/
    │   ├── Scripts/                          C++ script components (GameScripts project)
    │   ├── Shaders/
    │   └── Textures/
    │
    └── Default/                              Built-in engine assets (from RTBEngine_SDK)
        └── Icons/                            Editor panel icons
```

---

## 2. Architecture Overview

The editor is structured as two layers stacked on top of the engine:

```
┌────────────────────────────────────────────────────────────────────┐
│  EditorApplication                                                  │
│  ├── RTBEngine::Core::Application  (engine: window, GL, physics)   │
│  └── EditorLayer                   (ImGui docking + all panels)     │
│       ├── ToolbarPanel                                              │
│       ├── SceneHierarchyPanel                                       │
│       ├── InspectorPanel                                            │
│       ├── SceneViewPanel           (editor camera + framebuffer)    │
│       ├── GameViewPanel            (game camera + framebuffer)      │
│       ├── ContentBrowserPanel                                       │
│       └── ConsolePanel                                              │
└────────────────────────────────────────────────────────────────────┘
```

### Initialization Sequence

```
EditorApplication::Initialize()
  1. Project::Load("project.rtbproject")            ← read project config
  2. Application::Initialize()                       ← engine: window, GL, ResourceManager
  3. AudioSystem::Initialize()
  4. SceneManager callbacks registered
  5. Load GameScripts.dll                            ← register script component types
  6. EditorLayer::Initialize()                       ← create all panels
  7. Logger::AddCallback → ConsolePanel::AddLog      ← wire log output to console
  8. SceneManager::LoadScene(project.startScene)     ← open last scene
```

### Per-Frame Update

```
EditorApplication::Update()
  1. Handle pendingSceneLoad (from Content Browser double-click)
  2. Application::ProcessInput()                     ← SDL events → InputManager
  3. if (state == Play || state == Pause):
       Application::Update(deltaTime)                ← scene update, physics, audio
  4. RenderSceneToFramebuffer()
       a. SceneViewPanel::GetFramebuffer().Bind()
          Application::RenderShadowPass()
          Application::RenderGeometryPass(editorCamera)
          EditorGridRenderer::Render()
          ColliderRenderer::RenderSelection()
          FrameBuffer::Unbind()
       b. if (state == Play || state == Pause):
          GameViewPanel::GetFramebuffer().Bind()
          Application::RenderGeometryPass(gameCamera)
          CanvasSystem::RenderAll()
          FrameBuffer::Unbind()
  5. EditorLayer::Render()                           ← all ImGui panels
  6. Application::Window::SwapBuffers()
```

---

## 3. Editor State Machine

Defined in `Source/Core/EditorTypes.h`.

```cpp
enum class EditorState {
    Edit,   // Default. Scene can be modified. Physics and scripts are NOT running.
    Play,   // Engine runs at full speed. Scripts execute, physics simulates.
    Pause   // Same as Play but time is frozen. Runtime state is preserved.
};
```

### Transitions

| From | Action | To | What happens |
|------|--------|----|--------------|
| `Edit` | Play button | `Play` | `OnPlay()`: resets physics, re-initializes physics for scene, sets state = Play |
| `Play` | Pause button | `Pause` | `OnPause()`: sets state = Pause |
| `Pause` | Resume button | `Play` | `OnPause()` toggles back to Play |
| `Play` / `Pause` | Stop button | `Edit` | `OnStop()`: clears selection, reloads scene from disk |

**`OnPlay()`**:
```cpp
void EditorApplication::OnPlay() {
    state = EditorState::Play;
    uiLayer->SetEditorState(state);

    // Re-initialize physics with current scene state
    application->ResetPhysics();
    application->InitializePhysicsForScene(SceneManager::GetInstance().GetActiveScene());
}
```

**`OnStop()`**:
```cpp
void EditorApplication::OnStop() {
    state = EditorState::Edit;
    uiLayer->SetSelectedGameObject(nullptr);

    // Reload scene to discard runtime changes
    auto path = SceneManager::GetInstance().GetActiveScenePath();
    if (!path.empty())
        SceneManager::GetInstance().LoadScene(path);
}
```

---

## 4. EditorContext

`Source/UI/Panels/EditorPanel.h` — Shared state struct passed by reference to every panel's `OnUIRender()` call. Panels read from and write to this struct to communicate with each other.

```cpp
struct EditorContext {
    RTBEngine::ECS::GameObject* selectedGameObject = nullptr;
    EditorState                 state              = EditorState::Edit;
    std::filesystem::path       selectedAssetPath;
    std::filesystem::path       pendingSceneLoad;
};
```

| Field | Purpose |
|-------|---------|
| `selectedGameObject` | Which object is shown in the Inspector and highlighted in the Scene View |
| `state` | Current edit/play/pause state, used by panels to conditionally enable controls |
| `selectedAssetPath` | Path of the file selected in the Content Browser, shown in asset-ref fields |
| `pendingSceneLoad` | Set by Content Browser on double-click of a `.lua` file; consumed by `EditorApplication::Update()` |

### Panel Base Class

```cpp
class EditorPanel {
public:
    virtual ~EditorPanel() = default;
    virtual void OnUIRender(EditorContext& context) = 0;
};
```

Every panel receives the same `EditorContext&` reference, so changes made by one panel (e.g., Hierarchy setting `selectedGameObject`) are immediately visible to another (e.g., Inspector reading `selectedGameObject`).

---

## 5. EditorApplication

`Source/Core/EditorApplication.h` — Top-level class. Owns the engine `Application` and the `EditorLayer`. Implements the editor main loop.

### Constructor and Lifecycle

```cpp
EditorApplication();

bool Initialize();   // Returns false on failure
void Run();          // Blocks until RequestExit() is called
void Shutdown();
```

### Playback Control

```cpp
void OnPlay();    // Edit → Play
void OnPause();   // Play ↔ Pause toggle
void OnStop();    // Play/Pause → Edit, reloads scene
```

### Script Compilation

```cpp
void OnCompileScripts();
bool IsCompilingScripts() const;
```

`OnCompileScripts()` launches `BuildSystem::CompileScripts()` on a background `std::thread`. While compiling, `isCompilingScripts` is `true` and the Toolbar shows a progress indicator. When the thread finishes, the DLL is reloaded:

```cpp
// After compile succeeds:
FreeLibrary(gameScriptsDLL);
gameScriptsDLL = LoadLibrary("GameScripts.dll");
// Re-registration of component types happens via DLL static initializers
```

### Pending Scene Load

At the top of `Update()`, before any engine update:

```cpp
auto& ctx = uiLayer->GetContext();
if (!ctx.pendingSceneLoad.empty() && state == EditorState::Edit) {
    std::string requested = ctx.pendingSceneLoad.string();
    ctx.pendingSceneLoad.clear();

    const std::string& active = SceneManager::GetInstance().GetActiveScenePath();
    if (requested != active) {
        uiLayer->SetSelectedGameObject(nullptr);
        SceneManager::GetInstance().LoadScene(requested);
        return;   // Skip the rest of this frame
    }
}
```

This guard ensures:
1. Scene loads only happen in Edit mode.
2. Loading the already-active scene is a no-op.
3. The selection is cleared before the scene changes to avoid stale pointers.
4. If the scene changes while Play is running, the editor clears the current selection so the Inspector and gizmo state do not hold dangling references.

### Stop Behavior

When Play or Pause is stopped, the editor restores the original scene that was open when Play started. Runtime scene changes are discarded, selection is cleared, and the editor returns to the pre-Play scene instead of keeping the transient runtime scene.

### Rendering to Framebuffers

```cpp
void RenderSceneToFramebuffer();
```

Renders the scene twice per frame (when applicable): once to the Scene View framebuffer using the editor camera, and once to the Game View framebuffer using the in-game `CameraComponent`'s camera. The editor camera is retrieved from `SceneViewPanel::GetEditorCamera()`.

---

## 6. EditorLayer — Docking System

`Source/UI/EditorLayer.h` — Hosts the ImGui docking layout and owns all panel instances.

### Initialization

```cpp
void Initialize(RTBEngine::Core::Application* app);
void Render(EditorContext& context);
void Shutdown();
```

`Initialize()` creates all default panels and stores them in a `std::vector<std::unique_ptr<EditorPanel>>`. The default panels created are:

1. `SceneViewPanel`
2. `GameViewPanel`
3. `SceneHierarchyPanel`
4. `InspectorPanel`
5. `ContentBrowserPanel`
6. `ConsolePanel`

The toolbar is handled separately (not a standard `EditorPanel`).

### Dockspace Setup

```cpp
void SetupDockspace();
void CreateDefaultLayout();
```

`SetupDockspace()` creates a full-screen invisible ImGui window that acts as the docking host:

```cpp
ImGuiWindowFlags flags =
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoBringToFrontOnFocus |
    ImGuiWindowFlags_NoNavFocus |
    ImGuiWindowFlags_NoBackground;

ImGui::SetNextWindowPos(viewport->Pos);
ImGui::SetNextWindowSize(viewport->Size);
ImGui::Begin("DockSpace", nullptr, flags);
ImGui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2(0,0), ImGuiDockNodeFlags_PassthruCentralNode);
ImGui::End();
```

`CreateDefaultLayout()` uses `ImGui::DockBuilder` to pre-configure the panel layout on first run:

```
┌──────────────────────────────────────────────────────────────┐
│  Toolbar                                                      │
├───────────┬──────────────────────────────────┬───────────────┤
│           │                                  │               │
│ Hierarchy │         Scene View               │   Inspector   │
│           │                                  │               │
│           ├──────────────────┬───────────────┤               │
│           │ Content Browser  │   Console      │               │
└───────────┴──────────────────┴───────────────┴───────────────┘
```

Exact split ratios:

```cpp
ImGuiID dockMain = ImGui::GetID("MainDockSpace");
ImGuiID dockLeft, dockRight, dockCenter, dockBottom, dockBottomLeft, dockBottomRight;

ImGui::DockBuilderSplitNode(dockMain,   ImGuiDir_Left,  0.18f, &dockLeft,         &dockCenter);
ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, 0.28f, &dockRight,        &dockCenter);
ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down,  0.28f, &dockBottom,       &dockCenter);
ImGui::DockBuilderSplitNode(dockBottom, ImGuiDir_Left,  0.50f, &dockBottomLeft,   &dockBottomRight);
```

Panel assignments:

```cpp
ImGui::DockBuilderDockWindow("Scene Hierarchy", dockLeft);
ImGui::DockBuilderDockWindow("Scene View",      dockCenter);
ImGui::DockBuilderDockWindow("Game View",       dockCenter);     // Tab with Scene View
ImGui::DockBuilderDockWindow("Inspector",       dockRight);
ImGui::DockBuilderDockWindow("Content Browser", dockBottomLeft);
ImGui::DockBuilderDockWindow("Console",         dockBottomRight);
```

### Panel Management

```cpp
void AddPanel(std::unique_ptr<EditorPanel> panel);
```

Panels added after `Initialize()` are inserted into the render loop and drawn alongside the default panels.

### Selected Object Accessors

```cpp
RTBEngine::ECS::GameObject* GetSelectedGameObject() const;
void SetSelectedGameObject(RTBEngine::ECS::GameObject* go);
```

These forward to `context.selectedGameObject`. Used by `EditorApplication` to clear selection on Stop.

### Build Dialog

```cpp
void OpenBuildDialog();
```

Opens the `BuildDialog` ImGui modal on the next frame.

### Context Access

```cpp
EditorContext& GetContext();
```

Used by `EditorApplication::Update()` to check and consume `pendingSceneLoad`.

---

## 7. MainMenuBar

`Source/UI/MainMenuBar.h` — Renders the application menu bar at the top of the window.

### Menu Structure

**File**:
- **Save Scene** (`Ctrl+S`) — triggers `onSaveScene` callback if scene is dirty
- **Save Scene As...** (`Ctrl+Shift+S`) — triggers `onSaveSceneAs` callback
- **Build** (`Ctrl+B`) — opens `BuildDialog`
- **Exit** (`Alt+F4`) — triggers `onExit` callback

**Window**:
- Reserved for future panel toggles.

### Dirty Indicator

If `SceneManager::GetInstance().IsSceneDirty()` is true, the window title bar shows an asterisk (`*`) after the scene name to indicate unsaved changes.

### Callbacks

```cpp
struct MainMenuBarCallbacks {
    std::function<void()> onExit;
    std::function<void()> onBuild;
    std::function<void()> onSaveScene;
    std::function<void()> onSaveSceneAs;
};

MainMenuBar(MainMenuBarCallbacks callbacks);
void OnUIRender();
```

---

## 8. DragDropPayloads

`Source/UI/DragDropPayloads.h` — Constants and structs for ImGui drag-and-drop operations between panels.

### Payload Type Strings

```cpp
static constexpr const char* PAYLOAD_TEXTURE   = "ASSET_TEXTURE";
static constexpr const char* PAYLOAD_MESH      = "ASSET_MESH";
static constexpr const char* PAYLOAD_AUDIOCLIP = "ASSET_AUDIOCLIP";
static constexpr const char* PAYLOAD_FONT      = "ASSET_FONT";
static constexpr const char* PAYLOAD_CUBEMAP   = "ASSET_CUBEMAP";
static constexpr const char* PAYLOAD_GAMEOBJECT= "GAMEOBJECT";
```

### Payload Structs

```cpp
struct AssetPayload {
    char path[256];   // Relative asset path
};

struct GameObjectPayload {
    uint64_t gameObjectId;   // UUID of the dragged GameObject
};
```

### Usage Pattern

**Source** (Content Browser, dragging a texture):
```cpp
if (ImGui::BeginDragDropSource()) {
    AssetPayload payload;
    strncpy_s(payload.path, texturePath.c_str(), 255);
    ImGui::SetDragDropPayload(PAYLOAD_TEXTURE, &payload, sizeof(payload));
    ImGui::Text("Texture: %s", filename.c_str());
    ImGui::EndDragDropSource();
}
```

**Target** (Inspector, a TextureRef field):
```cpp
if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(PAYLOAD_TEXTURE)) {
        auto* payload = static_cast<const AssetPayload*>(p->Data);
        *reinterpret_cast<std::string*>(propPtr) = payload->path;
        component->OnValidate();
    }
    ImGui::EndDragDropTarget();
}
```

**Source** (Hierarchy, dragging a GameObject):
```cpp
if (ImGui::BeginDragDropSource()) {
    GameObjectPayload payload;
    payload.gameObjectId = gameObject->GetUUID();
    ImGui::SetDragDropPayload(PAYLOAD_GAMEOBJECT, &payload, sizeof(payload));
    ImGui::Text("%s", gameObject->GetName().c_str());
    ImGui::EndDragDropSource();
}
```

---

## 9. Scene Hierarchy Panel

`Source/UI/Panels/SceneHierarchyPanel.h` — Displays the full `GameObject` tree of the active scene with drag-and-drop reparenting, context menu creation, and keyboard deletion.

### Display

The panel shows the scene's name at the top (with a `*` if dirty), then renders each root `GameObject` as a collapsible tree node.

```
Scene: MainScene *
├── Directional Light
├── Player
│   ├── Body
│   └── Weapon
└── Floor
```

Inactive GameObjects are rendered with a dimmed text color. Selected GameObjects are highlighted with `ImGuiTreeNodeFlags_Selected`.

### Tree Node Rendering

```cpp
void DrawGameObjectNode(ECS::GameObject* go, EditorContext& context);
```

For each node:
1. Determines `ImGuiTreeNodeFlags` based on children count and selection state.
2. Calls `ImGui::TreeNodeEx`.
3. Handles drag-drop source (PAYLOAD_GAMEOBJECT).
4. Handles drag-drop target (PAYLOAD_GAMEOBJECT) for reparenting.
5. Handles selection on mouse release (not on drag start).
6. Recurses for each child.

**Selection rule**: Only change `context.selectedGameObject` when:
```cpp
if (ImGui::IsItemHovered() &&
    ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
    ImGui::GetDragDropPayload() == nullptr)
{
    context.selectedGameObject = gameObject;
}
```

This prevents the selection from changing when the user is in the middle of a drag operation.

**Deselect on background click**:
```cpp
if (ImGui::IsMouseDown(0) &&
    ImGui::IsWindowHovered() &&
    !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
{
    context.selectedGameObject = nullptr;
}
```

### Drag-and-Drop Reparenting

When a `PAYLOAD_GAMEOBJECT` is dropped onto another node, the dragged object's parent is changed:

```cpp
ECS::Scene* scene = SceneManager::GetInstance().GetActiveScene();
auto* dragged = scene->FindGameObjectByUUID(payload->gameObjectId);

// Cycle detection: don't allow dropping a parent onto its own descendant
if (!IsDescendantOf(target, dragged)) {
    dragged->SetParent(target);
    SceneManager::GetInstance().MarkSceneDirty();
}
```

`CollectDescendants(go, result)` recursively fills a vector with all children for cycle detection.

### Context Menu

Right-clicking empty space in the Hierarchy opens a context menu with:

**3D Primitives:**
- **Create Sphere** — `CreateSphere(scene)`: `AddGameObject("Sphere")` + `MeshRenderer` (default sphere mesh) + `RigidBodyComponent`
- **Create Cube** — `CreateCube(scene)`: same pattern with cube mesh
- **Create Plane** — `CreatePlane(scene)`: plane mesh, no RigidBody by default

**Effects:**
- **Create Particle System** — `CreateParticleSystem(scene)`: `AddGameObject("Particle System")` + `ParticleSystem` with cone emitter defaults, `playOnAwake = true`, and `simulateInEditMode = true` for Scene View preview

**UI Elements:**
- **Create Canvas** — `CreateCanvas(scene)`: `AddGameObject("Canvas")` + `Canvas` component (ScreenSpaceOverlay mode)
- **Create UIButton** — `CreateUIButton(scene)`: child of an existing Canvas (or creates one), `UIButton` with default label
- **Create UIText** — `CreateUIText(scene)`: `UIText` with placeholder string

**After creation**: Marks scene dirty, sets newly created object as selected.

### Deletion

```cpp
void DeleteGameObject(ECS::Scene* scene, ECS::GameObject* go);
void CollectDescendants(ECS::GameObject* go, std::vector<ECS::GameObject*>& result);
```

`Del` key on the selected object triggers deletion. `CollectDescendants` gathers all children and grandchildren. All descendants are removed before the root to avoid dangling parent pointers. Scene is marked dirty.

### Scene Settings

A collapsible "Scene Settings" header at the top of the panel exposes:
- **Skybox enabled** toggle
- **Skybox cubemap** drag-drop target (accepts `PAYLOAD_CUBEMAP`)

---

## 10. Inspector Panel

`Source/UI/Panels/InspectorPanel.h` — Displays and edits the properties of the selected `GameObject`. The most complex panel in the editor.

### Layout

```
[ Object Name field ]

[ Transform Section ]
  Position: [x] [y] [z]
  Rotation: [x] [y] [z]   (Euler degrees)
  Scale:    [x] [y] [z]

[ Component 1 Header ▼ ]           [×]
  property1: [widget]
  property2: [widget]

[ Component 2 Header ▼ ]           [×]
  ...

[ + Add Component ]
```

For UI objects (those with `UIElement`), the Transform section shows `RectTransform` fields instead of 3D transform fields.

### Name Editing

The GameObject name is shown in a full-width `ImGui::InputText` field. Changes are committed on Enter or on focus loss. The scene is marked dirty after any name change.

### Transform Section

**3D Transform:**
```cpp
// Position — direct drag
Vector3 pos = go->GetTransform().GetPosition();
if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
    go->GetTransform().SetPosition(pos);

// Rotation — stored as cached Euler angles to prevent quaternion drift
// rotationAngles[uuid] is a per-object float[3] cache
if (ImGui::DragFloat3("Rotation", rotationAngles[go], 0.1f)) {
    go->GetTransform().SetEulerAngles({rotX, rotY, rotZ});
}

// Scale
Vector3 scale = go->GetTransform().GetScale();
if (ImGui::DragFloat3("Scale", &scale.x, 0.01f))
    go->GetTransform().SetScale(scale);
```

**Why cache Euler angles?**
Converting quaternion → Euler → quaternion introduces floating-point rounding and gimbal artifacts. The Inspector stores the last user-typed Euler values per object UUID and only re-reads from the quaternion when the object's selection changes.

### Component Rendering

For each component on the selected object:

```cpp
void DrawComponents(ECS::GameObject* go, EditorContext& context);
```

1. Gets `TypeInfo` via `component->GetTypeInfo()`.
2. Opens a collapsible header with the component's display name.
3. Right-click on header → context menu with "Remove Component".
4. Calls `DrawProperty(component, prop, context)` for each inspector-visible property.

### Property Drawers

```cpp
void DrawProperty(ECS::Component* component,
                  const Reflection::PropertyInfo& prop,
                  EditorContext& context);
```

Computes the property address through `prop.GetMutableData(component)` and dispatches on `prop.type`:

| PropertyType | Widget | Notes |
|-------------|--------|-------|
| `Bool` | `ImGui::Checkbox` | |
| `Int` | `ImGui::DragInt` | |
| `Float` | `ImGui::DragFloat` or `ImGui::SliderFloat` | Slider if `rangeMin != rangeMax` |
| `Double` | `ImGui::DragScalar` (double) | |
| `String` | `ImGui::InputText` with 256-byte buffer | |
| `Vector2` | `ImGui::DragFloat2` | |
| `Vector3` | `ImGui::DragFloat3` | |
| `Vector4` | `ImGui::DragFloat4` | |
| `Quaternion` | Three `DragFloat` for Euler X/Y/Z | Converts to/from quat |
| `Color` | `ImGui::ColorEdit4` | |
| `Enum` | `ImGui::Combo` | Uses `prop.enumNames` |
| `AssetRef` | Typed path field + filtered asset browser + drag target | Uses `prop.assetType` to decide the allowed asset family |
| `TextureRef` | Text field + "…" button → AssetBrowserModal + Drag target | |
| `MeshRef` | Same pattern | |
| `AudioClipRef` | Same pattern | |
| `FontRef` | Same pattern | |
| `GameObjectRef` | Drag-drop target (PAYLOAD_GAMEOBJECT) | Shows target name |
| `ComponentRef` | Drag-drop target (PAYLOAD_GAMEOBJECT) | Resolves on UUID |

After any change: calls `component->OnValidate()` and marks scene dirty.

### Custom Component Drawers

Some engine components bypass the generic property loop and render dedicated controls first:

| Component | Controls |
|-----------|----------|
| `ParticleSystem` | **Play**, **Pause**, **Stop**, **Burst** (`burstCount`), live stats (active count, playback state), then all reflected properties |

`DrawParticleSystemComponent` calls `Play()`, `Pause()`, `Stop()`, or `Emit(burstCount)` from the toolbar buttons, then draws the reflected fields (`maxParticles`, `emissionRate`, emitter shape, colors, `textureRef`, `simulateInEditMode`, etc.). Any button change triggers `OnValidate()` and marks the scene dirty.

While the editor is in Edit mode, `EditorApplication::Update()` calls `ParticleSystem::TickScenePreview` so systems with `simulateInEditMode` and an active playback state animate in Scene View without pressing Play.

For typed script asset slots, the Inspector now uses the reflected `assetType` metadata instead of guessing from the property name. For example, a property registered with `RTB_PROPERTY_FBX(idleAnimationFbx)` renders as a dedicated FBX slot that accepts `.fbx` drag-and-drop from the Content Browser, opens the asset browser already filtered to FBX files, and stores the selected logical `Assets/...` path.

**GameObjectRef drag-drop target example:**

```cpp
// Display current target name or "(None)":
ECS::GameObject** goPtr = reinterpret_cast<ECS::GameObject**>(propPtr);
const char* targetName = *goPtr ? (*goPtr)->GetNameCStr() : "(None)";
ImGui::Text("%s", targetName);

// Accept drop:
if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(PAYLOAD_GAMEOBJECT)) {
        auto* payload = static_cast<const GameObjectPayload*>(p->Data);
        *goPtr = scene->FindGameObjectByUUID(payload->gameObjectId);
        component->OnValidate();
        SceneManager::GetInstance().MarkSceneDirty();
    }
    ImGui::EndDragDropTarget();
}
```

### Add Component

```cpp
// Bottom of component list:
if (ImGui::Button("+ Add Component")) {
    ImGui::OpenPopup("ComponentSearch");
}

if (ImGui::BeginPopup("ComponentSearch")) {
    static char filter[128] = "";
    ImGui::InputText("##filter", filter, 128);

    for (auto& [name, typeInfo] : TypeRegistry::GetInstance().GetRegisteredTypes()) {
        if (/* name matches filter */) {
            if (ImGui::Selectable(name.c_str())) {
                ECS::Component* comp = typeInfo->Create();
                go->AddComponent(comp);   // AddComponent with raw pointer variant
                SceneManager::GetInstance().MarkSceneDirty();
                ImGui::CloseCurrentPopup();
            }
        }
    }
    ImGui::EndPopup();
}
```

### Cubemap Asset Inspector

When `context.selectedAssetPath` ends with `.cubemap`, the Inspector shows a 6-face editor instead of GameObject properties.

```cpp
// Face order: Right, Left, Top, Bottom, Front, Back
static const char* faceNames[6] = {"Right (+X)", "Left (-X)", "Top (+Y)",
                                    "Bottom (-Y)", "Front (+Z)", "Back (-Z)"};

for (int i = 0; i < 6; i++) {
    ImGui::Text(faceNames[i]);
    ImGui::SameLine();
    ImGui::InputText(("##face" + std::to_string(i)).c_str(), facePaths[i], 256);
    ImGui::SameLine();
    if (ImGui::Button(("...##face" + std::to_string(i)).c_str()))
        assetBrowserModal.Open(AssetType::Texture, [&, i](std::string path) {
            strncpy_s(facePaths[i], path.c_str(), 255);
        });

    // Drag-drop texture onto face slot:
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(PAYLOAD_TEXTURE))
            strncpy_s(facePaths[i], static_cast<const AssetPayload*>(p->Data)->path, 255);
        ImGui::EndDragDropTarget();
    }
}

if (ImGui::Button("Save Cubemap Asset"))
    SaveCubemapAsset(cubemapPath, facePaths);
```

### Script Preview

When `context.selectedAssetPath` ends with `.h` or `.cpp`, the Inspector shows the first 40 lines of the file with syntax-highlighted text and an "Open in Editor" button that opens the active project's `GameScripts.vcxproj` (falling back to the selected file if the project file is missing).

### FormatTypeName

```cpp
std::string FormatTypeName(const std::string& typeName);
```

Inserts spaces before each uppercase letter for display: `"MeshRenderer"` → `"Mesh Renderer"`. Used for component header labels and the Add Component list.

---

## 11. Scene View Panel

`Source/UI/Panels/SceneViewPanel.h` — The 3D editor viewport. Renders the scene via an off-screen `FrameBuffer` using an independent editor camera, then displays the result as an ImGui texture.

### Framebuffer

```cpp
RTBEngine::Rendering::FrameBuffer& GetFramebuffer();
```

Created at 1280×720, dynamically resized when the panel is resized:

```cpp
ImVec2 panelSize = ImGui::GetContentRegionAvail();
if ((int)panelSize.x != framebuffer.GetWidth() || (int)panelSize.y != framebuffer.GetHeight()) {
    framebuffer.Resize((int)panelSize.x, (int)panelSize.y);
    editorCamera.SetAspectRatio(panelSize.x / panelSize.y);
}
```

### Editor Camera

```cpp
RTBEngine::Rendering::Camera* GetEditorCamera();
```

Initial state: position `{0, 2, 5}`, pitch `-20°`, yaw `180°`. This places the camera looking at the world origin from slightly above and in front.

### Toolbar

A secondary toolbar is drawn inside the panel (above the viewport image):

```
[ W Translate ] [ E Rotate ] [ R Scale ] | [ Local / World ]
```

Keyboard shortcuts:
- `W` → GizmoOperation::Translate
- `E` → GizmoOperation::Rotate
- `R` → GizmoOperation::Scale

These shortcuts are only processed when the Scene View is focused (not when typing in a text field).

### Editor Camera Control

Camera movement is active while right-clicking inside the viewport.

```cpp
void HandleMouseInput(ImVec2 viewportPos, ImVec2 viewportSize);
void HandleKeyboardInput(float deltaTime);
```

**Mouse look** (right mouse button held):
```cpp
if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
    float deltaX = ImGui::GetIO().MouseDelta.x;
    float deltaY = ImGui::GetIO().MouseDelta.y;
    editorCamera.Rotate(deltaY * sensitivity, deltaX * sensitivity);
}
```

**WASD movement** (while right mouse button is held):
```cpp
float speed = moveSpeed * deltaTime;
if (GetAsyncKeyState('W')) editorCamera.MoveForward(speed);
if (GetAsyncKeyState('S')) editorCamera.MoveForward(-speed);
if (GetAsyncKeyState('A')) editorCamera.MoveRight(-speed);
if (GetAsyncKeyState('D')) editorCamera.MoveRight(speed);
if (GetAsyncKeyState('E')) editorCamera.MoveUp(speed);
if (GetAsyncKeyState('Q')) editorCamera.MoveUp(-speed);
```

**Shift boost**: holding `Left Shift` multiplies speed by 3.

**Scroll to zoom**: `ImGui::GetIO().MouseWheel` moves the camera forward.

### Object Picking

```cpp
void HandleObjectPicking(EditorContext& context, ImVec2 viewportPos, ImVec2 viewportSize);
```

Fires on left-click inside the viewport (when no gizmo is hovered and no drag occurred):

```cpp
ImVec2 mousePos   = ImGui::GetMousePos();
ImVec2 relPos     = mousePos - viewportPos;

// Convert to normalized [0,1] then to RaycastUtils screen space:
Ray ray = RaycastUtils::ScreenPointToRay(
    {relPos.x, relPos.y},
    {viewportSize.x, viewportSize.y},
    *editorCamera
);

ECS::GameObject* closest = nullptr;
float            minDist = FLT_MAX;

for (auto& go : scene->GetGameObjects()) {
    if (!go->HasComponent<MeshRenderer>()) continue;

    // Union AABB over all meshes:
    Vector3 worldMin = go->GetWorldMatrix() * aabbMin;
    Vector3 worldMax = go->GetWorldMatrix() * aabbMax;

    float dist;
    if (RaycastUtils::IntersectsAABB(ray, worldMin, worldMax, dist)) {
        if (dist < minDist) {
            minDist = dist;
            closest = go.get();
        }
    }
}
context.selectedGameObject = closest;
```

### Gizmo System

Uses **ImGuizmo** (bundled with ImGui extras) for interactive 3D handles.

```cpp
enum class GizmoOperation { Translate, Rotate, Scale };

GizmoOperation gizmoOperation = GizmoOperation::Translate;
bool           gizmoLocalMode = true;   // Local vs World space
```

```cpp
void HandleGizmo(EditorContext& context, ImVec2 viewportPos, ImVec2 viewportSize);
```

```cpp
// Per frame:
ImGuizmo::SetOrthographic(false);
ImGuizmo::SetDrawlist();
ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

Matrix4 view       = editorCamera->GetViewMatrix();
Matrix4 proj       = editorCamera->GetProjectionMatrix();
Matrix4 worldModel = go->GetWorldMatrix();

ImGuizmo::OPERATION op = /* map GizmoOperation to ImGuizmo enum */;
ImGuizmo::MODE      mode = gizmoLocalMode ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

if (ImGuizmo::Manipulate(view.Data(), proj.Data(), op, mode, worldModel.Data())) {
    // Decompose result back to Transform:
    Vector3 translation, scale;
    Quaternion rotation;
    worldModel.Decompose(translation, rotation, scale);

    go->GetTransform().SetPosition(translation);
    go->GetTransform().SetRotation(rotation);
    go->GetTransform().SetScale(scale);
    SceneManager::GetInstance().MarkSceneDirty();
}
```

### View Cube

```cpp
void DrawViewCube(ImVec2 viewportPos, ImVec2 viewportSize);
```

A small 80×80 pixel interactive cube drawn in the top-right corner of the viewport. Clicking a face calls:

```cpp
void AlignCameraToView(const char* viewName);
```

View alignment options and resulting camera orientation:

| Face Clicked | Camera Position | Camera Look Direction |
|-------------|----------------|----------------------|
| Top | (0, dist, 0) | Looking down (-Y) |
| Bottom | (0, -dist, 0) | Looking up (+Y) |
| Front | (0, 0, dist) | Looking forward (-Z) |
| Back | (0, 0, -dist) | Looking backward (+Z) |
| Right | (dist, 0, 0) | Looking left (-X) |
| Left | (-dist, 0, 0) | Looking right (+X) |

`dist` is computed from the current camera distance to the scene origin, so the orbit radius is preserved.

### Panel State

```cpp
bool IsFocused() const;    // ImGui::IsWindowFocused()
bool IsHovered() const;    // ImGui::IsWindowHovered()
int  GetViewportWidth()  const;
int  GetViewportHeight() const;
```

### Grid and Collider Overlay

```cpp
EditorGridRenderer* GetGridRenderer();
ColliderRenderer*   GetColliderRenderer();
```

Both are rendered after the main geometry pass, before the framebuffer is unbound. The grid and axes are always visible. The collider wireframe is only drawn when an object with `BoxColliderComponent` is selected.

---

## 12. Game View Panel

`Source/UI/Panels/GameViewPanel.h` — Shows what the active in-game camera sees. Useful for checking camera framing without leaving the editor.

### Framebuffer

```cpp
RTBEngine::Rendering::FrameBuffer& GetFramebuffer();
int GetViewportWidth()  const;
int GetViewportHeight() const;
bool IsVisible() const;
```

Created at 1280×720, dynamically resized. The aspect ratio is passed to `Application` so the game camera matches the panel dimensions.

### Rendering

During `Play` and `Pause` states, `EditorApplication::RenderSceneToFramebuffer()` renders to this framebuffer using `Scene::GetActiveCamera()` (the `CameraComponent` marked as main).

During `Edit` state, the panel shows either the last rendered frame or a "Not Playing" overlay message.

### Canvas Overlay

The `CanvasSystem` is rendered on top of the game view framebuffer during Play/Pause. The system's screen-space offset is adjusted to match the Game View panel's position within the overall editor window:

```cpp
CanvasSystem::GetInstance().RenderAll(
    {(float)viewportWidth, (float)viewportHeight}
);
```

### Mouse Input Forwarding

When in Play mode, the Game View forwards pointer input to the scene UI only while the cursor is visible and free. If the game explicitly captures or hides the cursor, the editor stops forwarding pointer events until the cursor is released again.

```cpp
ImVec2 mousePos = ImGui::GetMousePos() - panelPos;
CanvasSystem::GetInstance().OnMouseMove(mousePos.x, mousePos.y);

if (ImGui::IsMouseClicked(0))
    CanvasSystem::GetInstance().OnMouseDown(0, mousePos.x, mousePos.y);
if (ImGui::IsMouseReleased(0))
    CanvasSystem::GetInstance().OnMouseUp(0, mousePos.x, mousePos.y);
```

Entering Play leaves the cursor visible and uncaptured. Pressing `Escape` in the Game View only releases and shows the cursor when the game had previously captured or hidden it; otherwise `Escape` does nothing at the host/editor level.

### UI Raycast Debug Overlay

When an object with a `UIElement` component is selected, the Game View draws red rectangles showing the hit-test regions of all `raycastTarget` elements — useful for debugging UI layouts.

---

## 13. Content Browser Panel

`Source/UI/Panels/ContentBrowserPanel.h` — A file browser rooted at the `Assets/` directory. The primary way to navigate and manage project assets.

### State

```cpp
std::filesystem::path currentDirectory;    // Currently browsed directory
std::filesystem::path selectedPath;        // Last clicked file
std::filesystem::path renamingPath;        // File currently being renamed (if any)
char                  renameBuffer[256];   // Text input for rename
```

### Layout

The panel renders a grid of icons. The number of columns is computed dynamically:

```cpp
float iconSize    = 64.0f;
float padding     = 16.0f;
float cellSize    = iconSize + padding;
float panelWidth  = ImGui::GetContentRegionAvail().x;
int   columnCount = std::max(1, (int)(panelWidth / cellSize));
ImGui::Columns(columnCount, nullptr, false);
```

### Icon Types

```cpp
enum class IconType { Folder, File, Lua, Model, Image, Shader, Cubemap };

IconType GetIconForFile(const std::filesystem::path& path);
```

Extension mapping:

| Extension | IconType |
|-----------|---------|
| (directory) | Folder |
| `.lua` | Lua |
| `.obj`, `.fbx`, `.dae`, `.gltf` | Model |
| `.png`, `.jpg`, `.jpeg`, `.bmp`, `.tga` | Image |
| `.vert`, `.frag`, `.glsl` | Shader |
| `.cubemap` | Cubemap |
| (other) | File |

Icons are loaded from `Default/Icons/` as PNG textures on panel initialization.

### Navigation

- **Click folder** → sets `currentDirectory` to that folder.
- **Back button** (`←`) → `currentDirectory = currentDirectory.parent_path()`.
- **Breadcrumb display** → shows path components as clickable links.

```cpp
void LoadIcons();
```

### File Interaction

**Single click**: sets `context.selectedAssetPath` to the file path. Other panels (Inspector) react to this.

**Double click**:
- Folder → navigate into it.
- `.lua` file → set `context.pendingSceneLoad = path` (consumed by `EditorApplication::Update`).
- Other files → no action (future: open in default app).

**Scene load guard** (in `EditorApplication::Update`):
```cpp
// Only load if:
// 1. In Edit state
// 2. Path is not empty
// 3. Path is different from currently loaded scene
```

### Drag-Drop Source

For each file, a drag-drop source is created based on the file type:

```cpp
if (ImGui::BeginDragDropSource()) {
    AssetPayload payload;
    strncpy_s(payload.path, relativePath.string().c_str(), 255);

    switch (iconType) {
        case IconType::Image:   ImGui::SetDragDropPayload(PAYLOAD_TEXTURE, ...);   break;
        case IconType::Model:   ImGui::SetDragDropPayload(PAYLOAD_MESH, ...);      break;
        case IconType::Lua:     /* Scene — no drag payload currently */             break;
        case IconType::Cubemap: ImGui::SetDragDropPayload(PAYLOAD_CUBEMAP, ...);   break;
    }
    ImGui::EndDragDropSource();
}
```

### Rename (F2)

Pressing `F2` on a selected file or folder opens an inline `InputText` widget in place of the filename label. Pressing `Enter` or clicking elsewhere commits the rename via `std::filesystem::rename()`.

### Context Menu (Right-Click)

```cpp
void DrawContextMenu(const std::filesystem::path& path, EditorContext& context);
```

Right-clicking empty space or a file shows:

**New:**
- **Folder** → creates a new directory `New Folder` (auto-numbered if exists).
- **C++ Component** → creates a `.h` + `.cpp` pair with the RTBEngine component boilerplate, saved into `Assets/Scripts/`.
- **C++ Class** → creates a bare `.h` + `.cpp` pair.
- **Scene** → creates a new empty `.lua` scene file.
- **Cubemap Asset** → creates a `.cubemap` text file with 6 empty face paths.

**File operations:**
- **Rename** → enters rename mode (same as F2).
- **Delete** → `std::filesystem::remove` (or `remove_all` for directories). Asks for confirmation.
- **Show in Explorer** → `ShellExecuteA(nullptr, "explore", folderPath, ...)`.

### C++ Component Boilerplate Generation

When "Create C++ Component" is selected, a dialog asks for the component name, then generates:

**Header** (`Assets/Scripts/ComponentName.h`):
```cpp
#pragma once
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class ComponentName : public RTBEngine::ECS::Component {
public:
    ComponentName();
    ~ComponentName() override;

    ComponentName(const ComponentName&) = delete;
    ComponentName& operator=(const ComponentName&) = delete;

    void OnAwake() override;
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnDestroy() override;

    RTB_COMPONENT(ComponentName)
};
```

**Implementation** (`Assets/Scripts/ComponentName.cpp`):
```cpp
#include "ComponentName.h"

using ThisClass = ComponentName;

RTB_REGISTER_COMPONENT(ComponentName)
RTB_END_REGISTER(ComponentName)

ComponentName::ComponentName() {}
ComponentName::~ComponentName() {}
void ComponentName::OnAwake() {}
void ComponentName::OnStart() {}
void ComponentName::OnUpdate(float deltaTime) {}
void ComponentName::OnFixedUpdate(float fixedDeltaTime) {}
void ComponentName::OnDestroy() {}
```

---

## 14. Console Panel

`Source/UI/Panels/ConsolePanel.h` — Displays log messages from `RTBEngine::Core::Logger` in real time.

### Data Model

```cpp
void AddLog(const RTBEngine::Core::LogMessage& message);
void Clear();
```

`AddLog` is registered as a `Logger::LogCallback` in `EditorApplication::Initialize()`. Every `RTB_INFO`, `RTB_WARN`, `RTB_ERROR` call in the engine or in script components triggers this callback on the same thread.

### Filters

Three toggle buttons at the top of the panel:

```cpp
bool filterInfo    = true;   // Show [INFO] messages (green button)
bool filterWarning = true;   // Show [WARN] messages (yellow button)
bool filterError   = true;   // Show [ERROR] messages (red button)
```

Rendered as colored `ImGui::Button` that toggle on click.

### Search

```cpp
char searchFilter[256] = "";
```

`ImGui::InputText("##search", searchFilter, 256)` — messages are only shown if `strstr(message.message.c_str(), searchFilter)` is non-null (case-sensitive).

### Auto-Scroll

```cpp
bool autoScroll = true;
```

When enabled: `if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);` — scrolls to the bottom after each new log entry.

### Message Display

Each log entry:

```
[HH:MM:SS] [i / ! / x] message text
```

Color coding:
- `Info` → `ImVec4(0.7f, 0.7f, 0.7f, 1.0f)` (gray)
- `Warning` → `ImVec4(1.0f, 0.8f, 0.0f, 1.0f)` (yellow)
- `Error` → `ImVec4(1.0f, 0.3f, 0.3f, 1.0f)` (red)

Icon per level: `[i]` (info), `[!]` (warning), `[x]` (error).

```cpp
float iconSize = 16.0f;   // Configurable for high-DPI displays
```

---

## 15. Toolbar Panel

`Source/UI/Panels/ToolbarPanel.h` — Contains playback buttons and the script compile button. Displays inline with the top dock area.

### Callbacks

```cpp
struct ToolbarCallbacks {
    std::function<void()>         onPlay;
    std::function<void()>         onPause;
    std::function<void()>         onStop;
    std::function<EditorState()>  getState;
    std::function<bool()>         isCompilingScripts;
    std::function<void()>         onCompileScripts;
};

ToolbarPanel(ToolbarCallbacks callbacks);
```

### Play / Pause / Stop Buttons

**Play button** (left-aligned):
- In `Edit` state: green "Play" button → calls `onPlay`.
- In `Play` state: grayed out.
- In `Pause` state: green "Play" button → calls `onPause` (resumes).

**Pause button**:
- In `Edit` state: disabled.
- In `Play` state: yellow "Pause" button → calls `onPause`.
- In `Pause` state: yellow "Resume" button → calls `onPause`.

**Stop button**:
- In `Edit` state: disabled.
- In `Play` / `Pause` state: red "Stop" button → calls `onStop`.

### Compile Scripts Button (right-aligned)

```cpp
// Right-align: calculate remaining space
float rightOffset = 160.0f;   // Approximate button width
ImGui::SetCursorPosX(ImGui::GetWindowWidth() - rightOffset);

if (isCompilingScripts()) {
    ImGui::BeginDisabled(true);
    ImGui::Button("Compiling...");
    ImGui::EndDisabled();
} else {
    ImGui::PushStyleColor(ImGuiCol_Button, compileButtonColor);
    if (ImGui::Button("Compile Scripts"))
        onCompileScripts();
    ImGui::PopStyleColor();
}
```

Button is disabled:
- While `isCompilingScripts()` is true.
- When `state != EditorState::Edit` (can't recompile while playing).

### Compilation Progress Modal

While compiling, a non-closeable modal is shown:

```cpp
ImGui::OpenPopup("Compiling Scripts");
if (ImGui::BeginPopupModal("Compiling Scripts", nullptr,
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
{
    ImGui::Text("Building GameScripts.dll...");

    // Animated progress bar (no actual progress reported):
    float t = (float)ImGui::GetTime();
    ImGui::ProgressBar(-1.0f * t, ImVec2(-1, 0), "");

    if (!isCompilingScripts())
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}
```

---

## 16. Build Dialog Modal

`Source/UI/Modals/BuildDialog.h` — An ImGui modal dialog for configuring and initiating a game build.

### BuildSettings Struct

```cpp
struct BuildSettings {
    std::string gameName        = "MyGame";
    std::string outputDirectory = "";
    std::string startScene      = "";
    int         windowWidth     = 1280;
    int         windowHeight    = 720;
    bool        fullscreen      = false;
};
```

### Dialog Layout

```
Game Name:          [_______________]
Output Directory:   [_______________] [Browse...]
Start Scene:        [_______________]
Window:             [1280] x [720]   [ ] Fullscreen

                              [Build] [Cancel]
```

The `Start Scene` field is chosen from the project scenes and persisted back into the project file before the build runs. The same value is written to `game.cfg` so the exported player starts in the selected scene.

The "Browse..." button invokes the Windows shell dialog:

```cpp
void DrawDirectorySelector(std::string& outPath) {
    // Uses IFileDialog (Vista+ shell COM dialog):
    IFileDialog* pfd;
    CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                     IID_IFileDialog, reinterpret_cast<void**>(&pfd));
    pfd->SetOptions(FOS_PICKFOLDERS);
    pfd->Show(nullptr);
    // ... get result path
}
```

### Build Progress

When "Build" is clicked, `BuildSystem::Build(settings, progressCallback)` is called synchronously. A progress bar is shown with a text status message from the `progressCallback`:

```cpp
BuildSystem::Build(settings, [&](const std::string& status) {
    buildStatus = status;   // Displayed above the progress bar
    buildProgress += 0.15f; // Approximate step increment
});
```

After completion, a result dialog is shown:

```
Build Successful!    or    Build Failed: [reason]
```

---

## 17. Asset Browser Modal

`Source/UI/Modals/AssetBrowserModal.h` — A compact file picker modal that filters by asset type. Used by Inspector property drawers.

### Asset Type Filter

```cpp
enum class AssetType { Texture, Mesh, AudioClip, Font, Cubemap, Fbx, Any };
```

Extension filters per type:

| AssetType | Accepted Extensions |
|----------|-------------------|
| Texture | `.png`, `.jpg`, `.jpeg`, `.bmp`, `.tga` |
| Mesh | `.obj`, `.fbx`, `.dae`, `.gltf` |
| AudioClip | `.wav`, `.mp3`, `.ogg`, `.flac` |
| Font | `.ttf`, `.otf` |
| Cubemap | `.cubemap` |
| Fbx | `.fbx` |
| Any | All files |

### Opening the Modal

```cpp
void Open(AssetType type, std::function<void(std::string path)> onSelected);
bool IsOpen() const;
void Render();
```

`Open()` stores the filter type and callback, then calls `ImGui::OpenPopup("Asset Browser")`.

### Layout

```
[ Assets ] [ Default ]    ← Root directories
[ back ]
──────────────────────────
📁 Textures/
📁 Models/
🖼 stone.png
🖼 wood.png
──────────────────────────
          [Cancel]
```

Navigation:
- Click `[Assets]` or `[Default]` → navigate to that root.
- Click a folder → enter it.
- Click `[back]` → go up one level.
- Double-click a file → calls `onSelected(relativePath)` and closes.

The modal uses `ImGui::BeginPopupModal` with `ImGuiWindowFlags_AlwaysAutoResize`, so it sizes to fit its content.

---

## 18. Build System

`Source/Build/BuildSystem.h` — All static methods. Packages a playable game from the current project.

### Build Process

```cpp
static BuildResult Build(const BuildSettings& settings,
                          std::function<void(const std::string&)> progressCallback = nullptr);
```

Internally calls these steps in order:

```
1. CreateDirectoryStructure(settings.outputDirectory)
   └── std::filesystem::create_directories

2. CopyPlayerExecutable(settings)
   └── Copy "RTBPlayer.exe" → "{outputDir}/{gameName}.exe"

3. CopyDLLs(settings)
   └── Copy all *.dll from "RTBEngine_SDK/Bin/" to outputDir

4. CopyDefaultFolder(settings)
   └── Recursively copy "Default/" → "{outputDir}/Default/"

5. CopyAssetsFolder(settings)
   └── Recursively copy "Assets/" → "{outputDir}/Assets/"

6. WriteGameConfig(settings)
   └── Write "{outputDir}/game.cfg"
```

### Game Config File Format

```ini
[Game]
name=MyGame

[Window]
width=1280
height=720
fullscreen=0

[Scene]
startScene=Assets/Scenes/Main.lua
```

This file is read by `RTBPlayer.exe` at startup to configure the `ApplicationConfig`.
The editor writes the selected start scene in `Assets/Scenes/X.lua` form, using forward slashes and without silently switching to another scene.

### Result Codes

```cpp
enum class BuildResult {
    Success,
    NoProjectLoaded,         // Project::GetActive() == nullptr
    InvalidOutputDirectory,  // outputDirectory is empty or creation failed
    PlayerNotFound,          // RTBPlayer.exe not found next to editor
    CopyFailed,              // File copy operation failed
    ConfigWriteFailed        // Could not write game.cfg
};

static std::string GetResultMessage(BuildResult result);
```

### Script Compilation

```cpp
static ScriptCompileResult CompileScripts(
    const std::string& vcxprojPath,
    const std::string& configuration = "Debug"
);
```

Invokes MSBuild:

```
"C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe"
    "{vcxprojPath}"
    /p:Configuration={configuration}
    /p:Platform=x64
    /t:Build
    /nologo
```

The path to MSBuild is hardcoded for VS 2026. Future improvement: discover via `vswhere.exe`.

```cpp
enum class ScriptCompileResult {
    Success,
    MSBuildNotFound,   // Executable not at expected path
    CompileError,      // MSBuild returned non-zero exit code
    Failure            // Process could not be created
};
```

---

## 19. Script Components

This section describes the complete workflow for creating, editing, and hot-reloading custom gameplay components in the editor.

### Overview

Script components live in `Assets/Scripts/` and are compiled into `GameScripts.dll`. The editor loads this DLL at startup and whenever the **Compile Scripts** button is pressed. The DLL registers its component types via `RTB_REGISTER_COMPONENT` static initializers, making them visible in the **Add Component** popup and serializable in scene files.

The script bridge is now ABI-safe: scripts pass plain descriptors to the engine, and the engine builds its own reflected metadata internally. Because of that, the SDK and `GameScripts.dll` need to be rebuilt after changes to the bridge, reflection macros, or public script headers.

### Creating a Component

Use the Content Browser context menu: right-click → New → C++ Component. Enter the name in the dialog. The editor generates the `.h` and `.cpp` files with the correct boilerplate (see [Content Browser Panel](#13-content-browser-panel)).

Alternatively, create the files manually following this template:

**Header** (`Assets/Scripts/Rotator.h`):

```cpp
#pragma once
#include <RTBEngine/ECS/Component.h>
#include <RTBEngine/Reflection/PropertyMacros.h>

class Rotator : public RTBEngine::ECS::Component {
public:
    Rotator();
    ~Rotator() override;

    Rotator(const Rotator&) = delete;
    Rotator& operator=(const Rotator&) = delete;

    //Loop methods
    void OnAwake() override;
    void OnStart() override;
    void OnUpdate(float deltaTime) override;
    void OnFixedUpdate(float fixedDeltaTime) override;
    void OnDestroy() override;

    // Reflected properties (Proxy)
    float speedRef = 90.0f;

    RTB_COMPONENT(Rotator)

private:
    float speed = 90.0f;
};
```

**Implementation** (`Assets/Scripts/Rotator.cpp`):

```cpp
#include "Rotator.h"
#include <RTBEngine/ECS/Transform.h>
#include <RTBEngine/ECS/GameObject.h>

using ThisClass = Rotator;

RTB_REGISTER_COMPONENT(Rotator)
    RTB_PROPERTY_RANGE(speedRef, -360.0f, 360.0f)
RTB_END_REGISTER(Rotator)

Rotator::Rotator() {}
Rotator::~Rotator() {}

void Rotator::OnAwake() {}

void Rotator::OnStart() {
    speed = speedRef;
}

void Rotator::OnUpdate(float deltaTime) {
    GetOwner()->GetTransform().RotateEuler({0, speed * deltaTime, 0});
}

void Rotator::OnFixedUpdate(float fixedDeltaTime) {}
void Rotator::OnDestroy() {}
```

### Typed Asset and Component Slots

Script components can now expose professional Inspector fields that are more structured than plain strings. Two common cases are:

- `RTB_PROPERTY_COMPONENT(animator, Animator)` for an explicit component reference assigned from the Hierarchy.
- `RTB_PROPERTY_FBX(idleAnimationFbx)` for a typed FBX asset slot assigned from the Content Browser.

Minimal example:

```cpp
class ThirdPersonCharacterController : public RTBEngine::ECS::Component {
public:
    RTBEngine::Animation::Animator* animator = nullptr;
    std::string idleAnimationFbx;
    std::string walkAnimationFbx;
    std::string runAnimationFbx;

    RTB_COMPONENT(ThirdPersonCharacterController)
};

using ThisClass = ThirdPersonCharacterController;

RTB_REGISTER_COMPONENT(ThirdPersonCharacterController)
    RTB_PROPERTY_COMPONENT(animator, Animator)
    RTB_PROPERTY_FBX(idleAnimationFbx)
    RTB_PROPERTY_FBX(walkAnimationFbx)
    RTB_PROPERTY_FBX(runAnimationFbx)
RTB_END_REGISTER(ThirdPersonCharacterController)
```

In the Inspector, `animator` shows up as a component reference, while the three animation fields become FBX-only slots with drag-and-drop and the `...` picker. Scene and prefab serialization still store the logical `Assets/...` paths, not absolute filesystem paths.

### UI Scene Buttons

The editor includes script components for main-menu style actions:

- `SceneChangeButton` exposes a `scenePath` field in the Inspector and requests a scene load when clicked. It is used by the `Play` button in `Assets/Scenes/MainMenu.lua`.
- The script checks the local `UIButton` state first. If the button is not interactable, it does nothing.
- `ButtonStyle` stays visual-only. It should not contain scene loading logic.

`SceneChangeButton` is assigned directly to a scene through its reflected field, so you do not need to type the path manually when placing the component.

### Application Quit Button

`ApplicationQuitButton` mirrors a Unity-style `Application.Quit` action:

- In runtime, it requests the application to close.
- In editor Play or Pause, it requests the editor to stop execution instead of closing the editor process.

### Adding to the vcxproj

The `GameScripts` Visual Studio project auto-discovers `Assets/Scripts/**/*.h` and `Assets/Scripts/**/*.cpp`, so new script files appear in the project without manual `.vcxproj` edits.

### Compiling and Hot-Reloading

1. Click **Compile Scripts** in the Toolbar.
2. The editor invokes MSBuild on the active project's `GameScripts.vcxproj`.
3. On success, the old `GameScripts.dll` is unloaded (`FreeLibrary`), the new one is loaded (`LoadLibrary`), and the static initializers re-register all component types.
4. The `GameScripts` project also mirrors the rebuilt DLL to the project root, so exported builds can pick up a freshly recompiled `GameScripts.dll` without manual file copies.
5. Components already in the scene keep their serialized property values (scene is reloaded from disk after stop, or properties are re-applied on next load).
6. If you touch the script bridge, reflection macros, or public engine headers used by scripts, rebuild the SDK first and then rebuild `GameScripts`.

### DLL Boundary Rules for Scripts

`GameScripts.dll` is compiled against the SDK headers, but the engine and scripts still need to respect the DLL boundary. Avoid moving ownership of STL objects across that line. The bridge is designed around plain data so the engine can rebuild its own metadata safely.

**Never do:**
```cpp
std::string name = gameObject->GetName();       // std::string crosses heap boundary
std::string msg  = "Hello " + name;             // operator+ uses wrong allocator
RTB_INFO(msg);                                  // std::string destructor frees wrong heap
```

**Always do:**
```cpp
// Use GetNameCStr() — returns const char* (no heap crossing):
const char* name = gameObject->GetNameCStr();

// Use snprintf into a stack buffer:
char msg[256];
snprintf(msg, sizeof(msg), "Hello %s", name);

// Pass const char* to RTB_INFO — uses engine's CRT safely:
RTB_INFO(msg);
```

**What is safe to pass:**
- All POD types (`int`, `float`, `bool`, raw pointers)
- `const char*` string literals and stack buffers
- Engine math types (`Vector2/3/4`, `Quaternion`, `Matrix4`) — pure float structs
- Raw `GameObject*`, `Component*`, `Scene*` pointers

**What is NOT safe to pass:**
- `std::string` (by value or reference)
- `std::vector<T>` (by value or reference)
- Any `std::unique_ptr`, `std::shared_ptr`
- Lambda captures that hold STL objects
- SDL types in public script-facing headers. SDL stays inside the engine implementation; script code should not need to include it.

### Component Lifecycle in Scene Loading

Understanding when properties are available is critical for correct script behavior:

```
SceneLoader::LoadScene():
  1. scene.AddGameObject("MyObject")
  2. go.AddComponent<Rotator>()          ← OnAwake fires HERE
                                            speedRef = 0.0f (default, not yet loaded)
  3. SceneReflectionUtils::Apply(...)    ← speedRef = 90.0f (set from scene file)
  4. component.OnValidate()
  ...all objects created...
  5. Deferred GameObjectRef resolution   ← targetRef pointers resolved by UUID
  ...first Update() tick...
  6. component.OnStart()                 ← speedRef = 90.0f (correct!)
```

**Rule**: never read reflected properties in `OnAwake`. Always use `OnStart` or `OnValidate`.

### Accessing Other Components

```cpp
void Rotator::OnStart() {
    // Get sibling component:
    auto* rb = GetOwner()->GetComponent<RTBEngine::ECS::RigidBodyComponent>();
    if (rb) {
        // ...
    }

    // Get component on another object:
    if (targetRef) {
        auto* mr = targetRef->GetComponent<RTBEngine::ECS::MeshRenderer>();
    }
}
```

### Collision Callbacks

```cpp
void Rotator::OnCollisionEnter(const RTBEngine::Physics::CollisionInfo& col)
{
    char buf[256];
    snprintf(buf, sizeof(buf), "Hit: %s", col.other->GetNameCStr());
    RTB_INFO(buf);
}
```

Requires a sibling `RigidBodyComponent` and `BoxColliderComponent` on the same GameObject for collision callbacks to fire.

---

## 20. Editor Rendering Utilities

### EditorGridRenderer

`Source/Rendering/EditorGridRenderer.h` — Renders an infinite-looking ground plane grid and colored XYZ axis lines into the Scene View framebuffer.

```cpp
void Initialize();
void Render(RTBEngine::Rendering::Camera* camera);
void Shutdown();

void SetGridSize(float size);       // Default: 100 units total extent
void SetGridSpacing(float spacing); // Default: 1 unit per grid line
void SetAxisLength(float length);   // Default: 10 units
```

**Grid rendering technique:**

The grid consists of horizontal and vertical lines from `-size/2` to `+size/2` in the XZ plane at Y=0. Lines are drawn using `GL_LINES`.

The grid snaps to the camera position (truncated to grid spacing) so it appears infinite as the camera moves:

```cpp
float snapX = std::floor(camera->GetPosition().x / spacing) * spacing;
float snapZ = std::floor(camera->GetPosition().z / spacing) * spacing;
// Grid center = (snapX, 0, snapZ)
```

**Rendering order:**
1. Grid is drawn with `glDepthMask(GL_FALSE)` (depth read but no write) — so geometry renders on top of it.
2. Axis lines are drawn with `glDepthMask(GL_TRUE)` — so they occlude geometry correctly.

**Axis colors:**
- X axis → `(1.0, 0.1, 0.1, 1.0)` (red)
- Y axis → `(0.1, 1.0, 0.1, 1.0)` (green)
- Z axis → `(0.1, 0.1, 1.0, 1.0)` (blue)

**Grid color**: `(0.4, 0.4, 0.4, 0.6)` (semi-transparent gray)

### ColliderRenderer

`Source/Rendering/ColliderRenderer.h` — Draws a wireframe box around the selected object's `BoxColliderComponent` for visual debugging.

```cpp
void Initialize();
void RenderSelection(RTBEngine::ECS::GameObject* selected,
                     RTBEngine::Rendering::Camera* camera);
void Shutdown();
```

**Technique:**

Creates 24 vertices (12 edges × 2 endpoints) for a unit cube, transformed by the `GameObject`'s world matrix and the collider's half-extents:

```cpp
Matrix4 transform = selected->GetWorldMatrix()
                  * Matrix4::Translation(collider->GetCenter())
                  * Matrix4::Scale(collider->GetSize() * 2.0f);
```

Rendered as `GL_LINES` with `blendEnabled = true`, `depthTest = true`, `depthMask = false`.

Wireframe color: `(0.1, 1.0, 0.0, 1.0)` (bright green).

---

## 21. Raycast Utilities

`Source/Utils/RaycastUtils.h` — Screen-space to world-space ray conversion and AABB intersection.

### Ray Struct

```cpp
struct Ray {
    RTBEngine::Math::Vector3 origin;
    RTBEngine::Math::Vector3 direction;   // Normalized
};
```

### ScreenPointToRay

```cpp
static Ray ScreenPointToRay(
    const RTBEngine::Math::Vector2& screenPos,
    const RTBEngine::Math::Vector2& viewportSize,
    const RTBEngine::Rendering::Camera& camera
);
```

Conversion steps:

```
1. screenPos → NDC: ndc.x = (screenPos.x / viewportSize.x) * 2 - 1
                    ndc.y = 1 - (screenPos.y / viewportSize.y) * 2  ← Y flip (0,0 = top-left)

2. NDC → clip space:  clip = Vector4(ndc.x, ndc.y, -1.0f, 1.0f)

3. clip → view space: viewSpace = Inverse(Projection) * clip
                      viewSpace.z = -1.0f
                      viewSpace.w =  0.0f   ← direction, not position

4. view → world:      worldDir = Inverse(View) * viewSpace
                      worldDir = worldDir.xyz().Normalized()

5. ray.origin    = camera.GetPosition()
   ray.direction = worldDir
```

### IntersectsAABB

```cpp
static bool IntersectsAABB(
    const Ray& ray,
    const RTBEngine::Math::Vector3& aabbMin,
    const RTBEngine::Math::Vector3& aabbMax,
    float& outDistance
);
```

Uses the **slab method** (Kay and Kajiya, 1986):

```cpp
for each axis (X, Y, Z):
    if ray.direction[axis] != 0:
        t1 = (aabbMin[axis] - ray.origin[axis]) / ray.direction[axis]
        t2 = (aabbMax[axis] - ray.origin[axis]) / ray.direction[axis]
        if t1 > t2: swap(t1, t2)
        tMin = max(tMin, t1)
        tMax = min(tMax, t2)
    else:
        if ray.origin[axis] < aabbMin[axis] || > aabbMax[axis]: return false

return tMin <= tMax && tMax > 0
outDistance = tMin > 0 ? tMin : tMax
```

---

## 22. Project System

`Source/Core/Project.h` — Stores project-level metadata (name, start scene, directories). Loaded from a `.rtbproject` file at editor startup.

### Project Struct / Class

```cpp
class Project {
public:
    std::string name;
    std::string startScene;
    std::filesystem::path projectDirectory;
    std::filesystem::path assetDirectory;

    bool Load(const std::filesystem::path& path);
    bool Save(const std::filesystem::path& path) const;

    static Project* GetActive();
    static void     SetActive(Project* project);

private:
    static Project* activeProject;
};
```

### File Format

A simple `key=value` text file (one setting per line):

```ini
name=My Game
startScene=Assets/Scenes/Main.lua
```

`assetDirectory` is derived from `projectDirectory + "/Assets"`.
`startScene` is normalized to `Assets/Scenes/X.lua` form with forward slashes when it is stored by the editor or written into build settings.

### Usage in Editor

- `EditorApplication::Initialize()` calls `Project::Load("project.rtbproject")`.
- If no project file exists, default values are used.
- `Project::Save()` is called when the user selects File → Save Project (not yet exposed in the menu; future feature).
- `BuildSystem::Build()` reads `Project::GetActive()->startScene` as the default value for `BuildSettings::startScene`.
- The selected start scene is persisted in the project file and reused by Build Settings.

---

## 23. Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+S` | Save scene (only if dirty) |
| `Ctrl+Shift+S` | Save scene as... |
| `Ctrl+B` | Open Build dialog |
| `W` (Scene View focused) | Gizmo: Translate |
| `E` (Scene View focused) | Gizmo: Rotate |
| `R` (Scene View focused) | Gizmo: Scale |
| `Right Mouse + WASD` | Move editor camera |
| `Right Mouse + Q/E` | Move camera down/up |
| `Left Shift` (while moving) | 3× camera speed |
| `Mouse Scroll` | Zoom camera |
| `F2` (Content Browser) | Rename selected file |
| `Del` (Scene Hierarchy) | Delete selected GameObject |
| `Del` (Content Browser) | Delete selected file |

---

## 24. Workflow Guides

### Creating and Configuring a Scene from Scratch

1. **Open the editor**. The default scene loads automatically.
2. In **Scene Hierarchy**, right-click → Create Cube. A "Cube" object appears.
3. In **Inspector**, expand Transform. Set Position to `{0, 0, 0}`.
4. Drag a texture from **Content Browser** onto the `textureRef` field in `MeshRenderer`.
5. Add a `RigidBodyComponent` via **+ Add Component**.
6. Set `bodyType` to Dynamic.
7. Add a `BoxColliderComponent`. Set `sizeRef` to `{0.5, 0.5, 0.5}`.
8. **File → Save Scene** (`Ctrl+S`).
9. Click **Play** to test physics simulation.
10. Click **Stop** to return to Edit mode.

### Adding a Script Component to an Object

1. In **Content Browser**, right-click → New → C++ Component. Name it `Spinner`.
2. Files `Assets/Scripts/Spinner.h` and `Spinner.cpp` are created.
3. Edit `Spinner.cpp`: in `OnUpdate`, add `GetOwner()->GetTransform().RotateEuler({0, 45.0f * deltaTime, 0});`.
4. Click **Compile Scripts**.
5. Select a GameObject in Hierarchy.
6. In Inspector, click **+ Add Component** and find `Spinner`.
7. Click **Play** — the object rotates.

### Setting Up Cross-Object References

1. Create two objects: "Sender" (with `Connector`) and "Receiver" (with `Connectable`).
2. Select "Sender". In Inspector, find `Connector`'s `targetRef` field.
3. Drag "Receiver" from the Hierarchy onto the `targetRef` field.
4. Save scene. Click Play. Console shows: `Object connected: Receiver`.

### Setting Up Third-Person Locomotion Animations

1. Add `Animator` and `ThirdPersonCharacterController` to the player object.
2. In the controller, drag the sibling `Animator` component into the `animator` field.
3. Drag the desired idle, walk, and run `.fbx` files from the Content Browser into `idleAnimationFbx`, `walkAnimationFbx`, and `runAnimationFbx`.
4. Save the scene. The component stores those selections as logical `Assets/...` references.
5. Enter Play mode. The controller registers the first clip from each FBX under its internal aliases and switches between them automatically based on locomotion state.

### Exporting a Build

1. Ensure all scenes, assets, and scripts are saved and compiled.
2. **File → Build** (`Ctrl+B`).
3. Enter game name and output directory.
4. Set window dimensions.
5. Click **Build**. The build copies the runtime files plus `Assets`, `Default`, and the compiled `GameScripts.dll`.
6. Navigate to the output directory. Run `{GameName}.exe`.

### Opening a Different Scene

- Double-click any `.lua` file in **Content Browser**.
- The editor switches to that scene on the next frame (only in Edit mode).
- If the currently open scene has unsaved changes, they are **discarded** — save first.

### Main Menu and Multiplayer Flow

1. Open `Assets/Scenes/MainMenu.lua` and press **Play**.
2. **Play** — solo game (`DefaultScene.lua`, no lobby).
3. **Multiplayer** — `MultiplayerMenu.lua` → choose **LAN Lobby** or **Online Lobby** → `LobbyScene.lua`.
4. In the lobby: create or join with a code; host presses **Start Game** when at least one remote player is connected.
5. In `DefaultScene.lua`: **Tab** opens the pause menu (game keeps running). **Resume** closes it; **Exit** returns to main menu (online exit notifies other players).

See [§25 Online and Multiplayer](#25-online-and-multiplayer) for scripts, scenes, and testing.

### Debugging Physics

1. Add `RigidBodyComponent` (Dynamic) and `BoxColliderComponent` to an object.
2. Run **Play**. The object should fall.
3. Select the object during Play. The **ColliderRenderer** draws a green wireframe around the collider volume in the Scene View.
4. Check the **Console** for any `[WARN]` from the physics system.
5. Common issues:
   - Object does not fall → check `bodyType` is `Dynamic`, not `Static`.
   - Object falls through floor → floor needs `RigidBodyComponent` (Static) + `BoxColliderComponent`.
   - Crash on scene reload → see Physics Lifecycle in engine README.

### Debugging the UI

1. Create a Canvas (ScreenSpaceOverlay) and add UIButton and UIText children in the Hierarchy.
2. Switch to **Game View** tab to see the UI overlay.
3. In Play mode, with the cursor visible, click inside the Game View to interact with the buttons.
4. The Game View draws red rectangles around `raycastTarget` elements for the selected UI object — useful for verifying hit-test regions.
5. Check Console for any `[WARN]` related to missing fonts or null Canvas references.

---

## 25. Online and Multiplayer

Game multiplayer uses the engine stack via `RTBEngine_SDK` (see engine README [§18](../../RTBEngine/RTBEngine/README.md#18-online-subsystem)). Logic lives in **`Assets/Scripts/`** → `GameScripts.dll`. The editor **Online** panel sets ports and relay URL only; **LAN vs Online** is chosen in-game from `MultiplayerMenu.lua`.

### 25.1 Menu Navigation

```
MainMenu.lua
├── Play          → DefaultScene.lua (solo)
├── Multiplayer   → MultiplayerMenu.lua
└── Exit          → quit

MultiplayerMenu.lua
├── LAN Lobby     → LobbyScene.lua  (SetSessionLobbyBackend: Lan)
├── Online Lobby  → LobbyScene.lua  (SetSessionLobbyBackend: Relay)
└── Back          → MainMenu.lua

LobbyScene.lua
├── Create / Join lobby
├── Start Game    → DefaultScene.lua (host only, ≥1 remote member)
└── Back          → MultiplayerMenu.lua (leaves lobby)

DefaultScene.lua
├── Tab           → pause menu (gameplay continues)
├── Resume        → close menu
└── Exit          → MainMenu (online: notify peers, despawn pawn)
```

### 25.2 Key Script Components

| Component | File | Purpose |
|-----------|------|---------|
| `MainMenuController` | `MainMenuController.cpp` | Play / Multiplayer / status messages |
| `MultiplayerMenuController` | `MultiplayerMenuController.cpp` | LAN vs Online backend preference |
| `LobbyMenuController` | `LobbyMenuController.cpp` | Create, join, start match |
| `OnlinePlayerManager` | `OnlinePlayerManager.cpp` | Spawn remote pawns, session profiles, despawn detect |
| `OnlineGameNetSubsystem` | `OnlineGameNetMessages.cpp` | Game RTBN messages (64+), match exit |
| `PauseMenuController` | `PauseMenuController.cpp` | Tab pause menu, exit match |
| `ResumeGameButton` | `ResumeGameButton.cpp` | UI click → resume |
| `ExitToMenuButton` | `ExitToMenuButton.cpp` | UI click → exit to main menu |

Message IDs: `Assets/Scripts/GameNetMessageIds.h`.

### 25.3 Scenes

| Scene | Controllers / notes |
|-------|---------------------|
| `MainMenu.lua` | `MainMenuController`, player name input |
| `MultiplayerMenu.lua` | `MultiplayerMenuController` |
| `LobbyScene.lua` | `LobbyMenuController` |
| `DefaultScene.lua` | `OnlinePlayerManager`, `PauseMenuController`, `Player GO` with `NetworkIdentity` + `NetworkTransform` |

**Scene order:** `OnlinePlayers` (manager) must be listed **before** `Player GO` in `DefaultScene.lua` so `OnStart` configures network identity before the first player tick.

### 25.4 Editor Online Panel

`Window → Online` (`OnlinePanel.cpp`):

| Setting | Purpose |
|---------|---------|
| Enabled | Initializes `OnlineSystem` on editor startup |
| LAN game port | UDP gameplay bind (use unique ports per local test instance) |
| LAN discovery port | UDP lobby discovery |
| Relay matchmaking URL | e.g. `http://localhost:8080/api/v1` |
| Default start scene | Scene for multiplayer test launcher |
| Multiplayer Test | Launches second instance with offset ports |

The panel does **not** replace in-game LAN/Online selection — that is handled by `MultiplayerMenuController`.

**Persistence:** `EditorOnlineSettings.json` via `EditorOnlineSettingsStore`.

### 25.5 Match Exit and Player Despawn

| Action | Behavior |
|--------|----------|
| Client **Exit** (pause menu) | Sends leave notice to host; leaves lobby; loads main menu |
| Host receives leave | Despawns client pawn locally; broadcasts `kMatchPlayerLeft` with `playerSlot` |
| Other clients | Despawn pawn for that slot; show `"Name has left the game"` |
| Host **Exit** | Broadcasts `kMatchHostAbandoned`; clients show message on main menu |
| Abrupt disconnect | Host `DetectAndDespawnDisconnectedPlayers()` when lobby member list drops (relay updates faster than LAN) |

### 25.6 Building and Testing

**Compile scripts after any `Assets/Scripts` change:**

```bat
RTBEngineEditor\RTBEngineEditor\GameScripts\build.bat
```

Or use toolbar **Compile Scripts** in the editor (close Play mode if DLL is locked).

**After engine online API changes:** run `RTBEngine/BuildSDK.bat`, then rebuild GameScripts.

**LAN — two instances on one PC:**

| Instance | Game port | Discovery port |
|----------|-----------|----------------|
| Editor / Player 1 | 27015 | 27016 |
| Player 2 / Multiplayer Test | 27017 | 27018 |

**Internet — relay:**

1. Start `RTBOnlineRelay` (see `RTBOnlineRelay/README.md`).
2. Set relay URL in Online panel.
3. In game: Multiplayer → Online Lobby → create/join → start.

**Game View:** click inside the Game View for pause menu buttons when the cursor is visible (not in mouse capture).

### 25.7 Test Checklist

- [ ] Solo **Play** from main menu — no lobby, normal movement.
- [ ] LAN lobby create/join and start with two instances.
- [ ] Online lobby via local relay (`docker compose up`).
- [ ] Each player sees their own camera on their pawn.
- [ ] Remote player movement visible on both screens.
- [ ] Tab pause: Resume closes menu; Exit returns to main menu.
- [ ] Client exit removes remote pawn on host and remaining clients.
- [ ] Host exit shows "The host abandoned the match" on clients.
- [ ] Console free of `[ERR]` from missing `GameScripts` registration (recompile DLL).
