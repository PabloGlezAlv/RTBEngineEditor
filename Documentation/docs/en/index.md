# RTBEngine

A C++17 3D engine for Windows, with a component-based visual editor. This site is the manual, the editor guide, and the scripting reference.

<div class="grid cards" markdown>

-   :material-book-open-page-variant:{ .lg .middle } **Manual**

    ---

    Architecture, scenes, scripting, physics, graphics, UI, and ECS.

    [:octicons-arrow-right-24: Open the manual](manual/index.md)

-   :material-view-dashboard:{ .lg .middle } **Editor**

    ---

    Hierarchy, Inspector, views, prefabs, shortcuts, and game builds.

    [:octicons-arrow-right-24: Editor guide](editor/index.md)

-   :material-lan:{ .lg .middle } **Online**

    ---

    LAN lobbies, the HTTP/UDP relay, and network components.

    [:octicons-arrow-right-24: Multiplayer](online/index.md)

-   :material-code-braces:{ .lg .middle } **Scripting API**

    ---

    Classes, methods, and `GameScripts` examples.

    [:octicons-arrow-right-24: Reference](api/index.md)

</div>

## Three pieces

| Piece | What it is | Version |
| --- | --- | --- |
| `RTBEngine.dll` | Engine: scene, render, physics, audio, net | 1.0.0 |
| `RTBEngineEditor` | ImGui editor on top of the SDK | 1.0.0 |
| `RTBOnlineRelay` | HTTP matchmaking and UDP relay | API `/api/v1` |

Authoring lives in `RTBEngine::Scene` (GameObject and Component). Dense simulation lives in `RTBEngine::ECS`. The game does not reimplement the engine: it registers components in `GameScripts.dll`.

## Where to go

1. [What is RTBEngine](manual/que-es.md) for the map of the project.
2. [Installation](manual/instalacion.md) to build the engine and the editor.
3. [Your first game](manual/primer-juego.md) if the editor is already open.
4. [C++ components](manual/scripting/componentes-cpp.md) for the first script.

!!! note "Language"
    English is served under `/en/`. Spanish is the default language at the site root.
