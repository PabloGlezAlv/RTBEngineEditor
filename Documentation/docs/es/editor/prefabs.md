# Prefabs en el editor

Doble clic en un `.prefab`, o **Open Prefab** en el Inspector, abre una escena de staging. El nivel sigue en memoria y no se escribe.

## Sesión

1. Se crea una escena que no pasa a ser la activa.
2. Se instancia el prefab **sin** regenerar UUID, para que el archivo sea estable.
3. Se despiertan los componentes de esa raíz.
4. Se añade una luz de editor `__PrefabEditorLight` que no forma parte del asset.
5. La jerarquía selecciona la raíz.

**Save** (`Ctrl+S`) captura la raíz, escribe el `.prefab` y recarga el registro. **Back to Scene** destruye la staging y la luz. Si hay cambios sin guardar al abrir otro prefab, el editor pregunta guardar, descartar o cancelar.

## Qué sigue funcionando

Jerarquía, Scene View, Inspector, copiar y pegar y el gizmo operan sobre la staging. Partículas con `simulateInEditMode` y el preview del animator avanzan. Los scripts de juego no. **Play** está desactivado.

## Instancias en el nivel

Fuera del modo prefab, una instancia colocada en la escena enseña overrides. Apply escribe el `.prefab` en disco. Revert lee el asset. Revert All reinstancia y conserva nombre, UUID, padre y activo.

## Límite conocido

`Animator::OnValidate` sigue creando huesos contra la escena activa, no contra la staging. No dependas de GameObjects de hueso como parte autorada del prefab.
