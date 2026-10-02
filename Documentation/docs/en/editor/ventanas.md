# Optional windows

**Window** opens panels that start closed. Closing with X does not leave an empty tab: the panel stops calling `Begin`. When you open it again, it docks into a dock that already exists (another optional window, Inspector, Hierarchy, Console, Content Browser, Scene, or Game).

They are remembered in `%LOCALAPPDATA%\RTBEngineEditor\EditorWindowPrefs.json`.

| Menu | Purpose |
| --- | --- |
| Online | LAN ports, relay URL, launch a second instance |
| Physics Layers | Names and the collision matrix |
| Navigation Debug | Grid overlay in the Scene View |
| Project Settings | Graphics API and lighting |

The Online menu does **not** choose LAN or Online for the match. The in-game menu does that. This panel only prepares ports and the URL. The flow is in [LAN match](../online/lan.md).

Navigation Debug does not bake. Baking stays on `NavGridComponent`. Physics Layers does write the project when you press Save.

Each panel's visibility and the nav-debug toggles are saved on editor shutdown, on Window menu changes, and when you edit the overlay.
