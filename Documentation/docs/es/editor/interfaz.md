# La interfaz

Al abrir el editor el layout por defecto es este:

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

Los paneles se pueden desacoplar. ImGui guarda posiciones en `imgui.ini`. Las ventanas opcionales no ocupan sitio hasta que las abres en **Window**.

## Arranque

1. `Project::Load` del `.rtbproj` que está junto al ejecutable.
2. `Application::Initialize` (ventana, RHI, recursos).
3. Audio.
4. Carga de `GameScripts.dll` y registro de componentes.
5. Paneles.
6. El log del motor se enchufa a la consola.
7. Se abre `LastOpenScene` o la escena de inicio.

## Estados

| Estado | Escena | Scripts y física |
| --- | --- | --- |
| Edit | Se puede editar | No corren |
| Play | Runtime | Corren a velocidad normal |
| Pause | Runtime congelado | El estado se conserva, el tiempo no avanza |

| Desde | Acción | Hacia |
| --- | --- | --- |
| Edit | Play | Play. Reinicia la física de la escena |
| Play | Pause | Pause |
| Pause | Play | Vuelve a Play |
| Play o Pause | Stop | Edit. Limpia la selección y recarga la escena de disco |

Play está desactivado mientras editas un prefab.

## Contexto compartido

Todos los paneles leen el mismo `EditorContext`: objeto seleccionado, selección múltiple, estado, asset seleccionado, escena o prefab pendiente de abrir, ajustes de nav debug y qué ventanas opcionales están abiertas.

`GetEditingScene()` devuelve la escena de staging si hay un prefab abierto, y si no la escena activa. Jerarquía, Inspector, gizmos y el render usan esa función, así que el nivel no se modifica mientras editas un prefab.

## Título de la ventana

Si la escena o el prefab tienen cambios sin guardar, el título lleva `*`. `Ctrl+S` guarda la escena, o el prefab si estás en modo prefab. `Ctrl+Shift+S` es Guardar escena como. `Ctrl+B` abre el build. Salir es el cierre de la ventana.
