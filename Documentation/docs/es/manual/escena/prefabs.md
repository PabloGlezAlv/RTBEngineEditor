# Prefabs

Un prefab es un `.prefab` con la misma forma Lua que un GameObject de escena: raíz, hijos y componentes. Vive en `Assets/Prefabs/` y se instancia las veces que haga falta.

```text
Assets/Prefabs/
  Player/Gameplay/     peones de partida
  Player/Preview/      selección de personaje
  Enemies/
  Combat/Projectiles/
  Combat/Effects/
```

## Instanciar

```cpp
RTBEngine::Scene::Prefab prefab;
// Tras cargar el asset (PrefabRegistry::Load en el motor / editor):
RTBEngine::Scene::GameObject* pawn =
    RTBEngine::Scene::SceneManager::GetInstance().Instantiate(prefab, parent, true);
```

`regenerateUuids = true` da identidades nuevas. Es lo correcto al soltar varias copias en un nivel. `false` conserva los UUID del archivo: lo usa el modo de edición de prefab para que el guardado sea redondo.

## Instancias en una escena

Una instancia recuerda `GetPrefabName()`. En el nivel puedes cambiar propiedades sin editar el asset:

- Una propiedad distinta del prefab queda como **override**.
- **Revert** restaura ese campo desde el asset.
- **Apply** escribe ese campo en el `.prefab` y recarga el registro.
- **Revert All** reinstancia el prefab y conserva nombre, UUID, padre y estado activo de la instancia.
- **Unlink** deja de ser instancia.

El Inspector marca en azul los campos con override. Clic derecho en la etiqueta: Revert o Apply de ese campo. El transform (posición, rotación, escala) usa el mismo menú.

## Editar el asset

Doble clic en el `.prefab` del Content Browser abre una escena de staging. El nivel sigue cargado y no se toca. **Play** está bloqueado. `Ctrl+S` guarda el prefab. **Back to Scene** cierra la staging.

El detalle de sesión está en [Prefabs en el editor](../../editor/prefabs.md).

## Pool

Los proyectiles y efectos no deberían hacer `Instantiate` y destruir cada disparo. [Object Pool](object-pool.md) recicla instancias del mismo prefab.
