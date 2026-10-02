# Inspector

El Inspector edita el `GameObject` seleccionado o, si no hay objeto y sí un asset seleccionado, ese asset.

## Objeto

1. Nombre. Se confirma al salir del campo.
2. Transform: posición, rotación en **grados**, escala. En objetos de UI, el rect.
3. Un bloque por componente, con quitar en el menú contextual de la cabecera.
4. **+ Add Component**, con filtro por nombre.

La rotación en grados se cachea por UUID mientras el objeto sigue seleccionado. Volver a leer el quaternion cada frame introduciría deriva. El valor que acaba en C++ son radianes; la conversión es del Inspector. Ver [Matemáticas](../manual/matematicas.md).

## Campos

| Tipo | Control |
| --- | --- |
| Bool | Checkbox |
| Int / float | Drag. Slider si la propiedad tiene rango |
| String | Texto |
| Vector2/3/4 | Drags |
| Color | Selector |
| Enum | Combo |
| Textura, malla, audio, fuente, FBX | Texto, botón `...`, drop del tipo correcto |
| GameObject | Drop desde la jerarquía. Muestra el nombre o `(None)` |
| Component | Igual, y resuelve el componente en ese objeto |

Cualquier cambio llama `OnValidate` y marca dirty la escena o el prefab.

## Drawers propios

| Componente | Además de los campos |
| --- | --- |
| `Animator` | `modelRef`, modelos extra. Al cambiarlos, `ReloadClipLibrary` |
| `ParticleSystem` | Play, Pause, Stop, Burst y contadores |
| `NavGridComponent` | Bake Grid, Clear Baked, celdas walkable |
| `VolumeComponent` | Zona y nieblas, no la lista genérica |

## Instancia de prefab

Si el objeto viene de un prefab verás la cabecera **Prefab** (nombre, Select Root, Revert All, Unlink) y las etiquetas azules de los overrides. Clic derecho en una etiqueta: Revert o Apply de ese campo. Un componente que no está en el asset sale como **Added Component**.

## Assets

| Selección | Inspector |
| --- | --- |
| `.cubemap` | Seis caras: Right, Left, Top, Bottom, Front, Back, y **Save** |
| `.h` / `.cpp` | Primeras líneas y **Open in Editor** (abre el `GameScripts.vcxproj`) |
| `.rtbasset` | Propiedades del data asset |

**Open Prefab** en un `.prefab` seleccionado entra en el modo de edición.
