# Ciclo de vida

`SceneLifecycle` llama estos métodos. No los invoques tú.

| Método | Momento |
| --- | --- |
| `OnAwake` | El componente se añade al objeto. Todavía no hay datos de escena |
| `OnEnable` | El objeto está activo en jerarquía y el componente está enabled |
| `OnValidate` | Tras aplicar propiedades, y cuando el Inspector edita un campo |
| `OnStart` | Primer tick, una sola vez, con propiedades y referencias ya resueltas |
| `OnUpdate` | Cada frame |
| `OnFixedUpdate` | Paso de física |
| `OnLateUpdate` | Después de física y del tick ECS de simulación |
| `OnDisable` | El componente o el objeto dejan de estar activos |
| `OnDestroy` | El componente se quita o el objeto se destruye |
| `OnParentChanged` | Cambia el padre del dueño |

También existen `OnCollisionEnter` / `Stay` / `Exit` y `OnTriggerEnter` / `Stay` / `Exit`. Hace falta un cuerpo y un collider en el mismo objeto. Ver [Colisiones](../fisica/colisiones.md).

## Carga de escena, en orden

```text
AddComponent          OnAwake          speedRef == valor del constructor
Apply propiedades     OnValidate       speedRef == valor del .lua
Resolver UUID         —                los GameObject* ya apuntan
primer Update         OnStart          aquí lees el estado de autor
```

```cpp
void Spinner::OnAwake() {
    // speedRef puede ser 0 o el default. No configures gameplay aquí.
}

void Spinner::OnStart() {
    speed = speedRef;
    auto* body = GetOwner()->GetComponent<RTBEngine::Scene::RigidBodyComponent>();
    if (body && body->GetRigidBody()) {
        // el cuerpo Bullet ya existe tras el init de física de la escena
    }
}
```

## Enable

`SetActive(false)` en el GameObject desactiva la jerarquía y llama `OnDisable` en los componentes que estaban activos. `SetEnabled(false)` en un solo componente hace lo mismo para ese componente. Al reactivar, `OnEnable`. `OnStart` no se repite.

`OnDestroy` es el sitio para cancelar trabajo propio. Las acciones latentes del componente se cancelan solas al destruirlo.

## Edit contra Play

| | Edit | Play |
| --- | --- | --- |
| `OnAwake` / `OnStart` / `OnUpdate` de scripts | No corren como simulación | Sí |
| `OnValidate` | Sí, al editar | Sí, si algo reescribe propiedades |
| `OnEditModeSimulate` | Solo si el componente lo pide | No hace falta |

Parar Play recarga la escena. Todo lo que hayas cambiado en memoria durante Play desaparece, salvo que lo hayas escrito a disco por otra vía.
