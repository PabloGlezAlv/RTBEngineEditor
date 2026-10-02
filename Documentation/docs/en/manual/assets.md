# Assets

Everything the game loads at runtime lives in `Assets/` or in `Default/` (factory resources the SDK copies). Paths stored by scenes are logical: `Assets/Textures/Stone.png`, with `/` separators.

## Extensions

| Extension | What it is | Content Browser drag |
| --- | --- | --- |
| `.lua` | Scene | Double-click loads the scene (Edit only) |
| `.prefab` | Prefab | Double-click opens prefab mode |
| `.rtbasset` | Reflected data asset | The Inspector edits it |
| `.rtbproj` | Editor project | Not a scene asset |
| `.navmesh` | Baked grid next to the scene | Written by the bake |
| `.cubemap` | Six skybox faces | Cubemap payload |
| `.png` `.jpg` `.jpeg` `.bmp` `.tga` | Texture | Texture payload |
| `.obj` `.fbx` `.dae` `.gltf` | Mesh | Mesh payload |
| `.fbx` | Also clip / rig | `RTB_PROPERTY_FBX` slot |
| `.wav` `.mp3` `.ogg` `.flac` | Clip | Audio payload |
| `.ttf` `.otf` | UI font | Font payload |
| `.vert` `.frag` `.glsl` | Shader | — |
| `.h` `.cpp` | Script | The Inspector shows a preview |

## Default

`Default/` ships shaders, primitive meshes, and editor icons (`Default/Icons/`). The build copies it whole alongside `Assets/`. Do not put your game content there: the SDK can regenerate it.

## Inspector asset browser

The `...` button on a field opens a filtered modal:

| Filter | Extensions |
| --- | --- |
| Texture | png, jpg, jpeg, bmp, tga |
| Mesh | obj, fbx, dae, gltf |
| AudioClip | wav, mp3, ogg, flac |
| Font | ttf, otf |
| Cubemap | cubemap |
| Fbx | fbx |

Pick the file and the field stores the relative path. You can also drop the file onto the field if the payload matches.

## Project

`MyProject.rtbproj` sits next to the editor executable:

```ini
Name=My Game
StartScene=Assets/Scenes/Main.lua
LastOpenScene=Assets/Scenes/DefaultScene.lua
AssetDirectory=Assets
GraphicsAPI=Vulkan
```

`lighting.ini` sits beside it and stores ambient, shadows, DDGI, and fog. Do not mix those values into the `.rtbproj`.
