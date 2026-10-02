# The interface

When you open the editor, the default layout is this:

```text
┌──────────────────────────────────────────────────────────┐
│ Toolbar                                                  │
├────────────┬─────────────────────────────┬───────────────┤
│            │                             │               │
│ Hierarchy  │ Scene View / Game View      │ Inspector     │
│            │                             │               │
│            ├──────────────┬──────────────┤               │
│            │ Content      │ Console      │               │
└────────────┴──────────────┴──────────────┴───────────────┘
```

Panels can be undocked. ImGui stores positions in `imgui.ini`. Optional windows take no space until you open them from **Window**.

## Startup

1. `Project::Load` of the `.rtbproj` that sits next to the executable.
2. `Application::Initialize` (window, RHI, resources).
3. Audio.
4. Load `GameScripts.dll` and register components.
5. Panels.
6. The engine log is wired into the console.
7. `LastOpenScene` opens, or the start scene.

## States

| State | Scene | Scripts and physics |
| --- | --- | --- |
| Edit | Can be edited | Do not run |
| Play | Runtime | Run at normal speed |
| Pause | Frozen runtime | State is kept; time does not advance |

| From | Action | To |
| --- | --- | --- |
| Edit | Play | Play. Restarts the scene physics |
| Play | Pause | Pause |
| Pause | Play | Returns to Play |
| Play or Pause | Stop | Edit. Clears the selection and reloads the scene from disk |

Play is disabled while you edit a prefab.

## Shared context

Every panel reads the same `EditorContext`: the selected object, the multi-selection, the state, the selected asset, a scene or prefab waiting to be opened, nav debug settings, and which optional windows are open.

`GetEditingScene()` returns the staging scene when a prefab is open, and the active scene otherwise. Hierarchy, Inspector, gizmos, and rendering use that function, so the level stays as it is while you edit a prefab.

## Window title

If the scene or the prefab has unsaved changes, the title carries `*`. `Ctrl+S` saves the scene, or the prefab if you are in prefab mode. `Ctrl+Shift+S` is Save Scene As. `Ctrl+B` opens the build. Exiting is closing the window.
