# Component

`RTBEngine::Scene::Component` es la base de los componentes del motor y de los scripts de `GameScripts`. El motor llama los mensajes; un script no los invoca a mano. La copia está borrada.

## OnAwake

```cpp
virtual void OnAwake();
```

El motor lo llama una vez, cuando el componente entra en la escena y todavía no se han aplicado las propiedades del archivo ni se han resuelto los UUID. Sirve para reservar estado propio. El cuerpo de física del dueño aún no existe, y un `GetComponent` de un hermano puede devolver null si ese hermano todavía no ha despertado.

## OnEnable

```cpp
virtual void OnEnable();
```

Se llama cuando el componente pasa a estar activo en la jerarquía: `IsEnabled()` es true y el `GameObject` dueño está activo, incluidos sus padres. Ocurre al cargar, al reactivar el objeto y al volver a habilitar el componente. Aquí puedes suscribirte a cosas que solo deben vivir mientras el objeto participa en el juego.

## OnDisable

```cpp
virtual void OnDisable();
```

Se llama cuando el componente deja de estar activo en la jerarquía, porque se deshabilitó, porque el dueño o un padre pasó a inactivo, o porque va a destruirse. Es el sitio para soltar suscripciones hechas en `OnEnable`. No destruyas el dueño desde aquí.

## OnStart

```cpp
virtual void OnStart();
```

Se llama una sola vez, después de `OnAwake`, cuando las propiedades reflejadas ya tienen el valor de la escena y los UUID de referencias ya apuntan a objetos. Si el componente o el dueño nacen desactivados, espera a la primera vez que queden activos. Lee otros componentes y configura el gameplay aquí, no en `OnAwake`.

## OnUpdate

```cpp
virtual void OnUpdate(float deltaTime);
```

Se llama cada frame de juego mientras el componente está activo y su tick de update no está apagado. `deltaTime` son los segundos desde el frame anterior, a escala de tiempo del modo del componente.

- `deltaTime`: segundos de este frame.

## OnFixedUpdate

```cpp
virtual void OnFixedUpdate(float fixedDeltaTime);
```

Se llama en el paso de física, con un intervalo fijo, no una vez por frame de render. Úsalo para fuerzas y para leer contactos que deben coincidir con Bullet.

- `fixedDeltaTime`: segundos de este paso de física.

## OnLateUpdate

```cpp
virtual void OnLateUpdate(float deltaTime);
```

Se llama cada frame después de `OnUpdate`, de la física y del tick ECS de simulación. Sirve para leer un impacto ya resuelto, aplicar daño, VFX o audio, y para seguir a otro transform que ya se movió en `OnUpdate`.

- `deltaTime`: segundos de este frame.

## OnValidate

```cpp
virtual void OnValidate();
```

El editor y el loader lo llaman cuando cambian propiedades en el Inspector o al deserializar. Ajusta campos derivados (clampear un rango, recalcular un collider). No lances gameplay: en Edit no hay partida.

## OnDestroy

```cpp
virtual void OnDestroy();
```

Se llama cuando el componente se quita o cuando el dueño se destruye. Las acciones latentes de este componente se cancelan en este punto. No uses el dueño después de volver: puede estar a medio destruir.

## OnParentChanged

```cpp
virtual void OnParentChanged(GameObject* oldParent, GameObject* newParent);
```

Se llama en los componentes del objeto cuyo padre acaba de cambiar, después de `SetParent`. `oldParent` es el padre anterior y `newParent` el nuevo. Cualquiera de los dos puede ser null: null en `oldParent` significa que el objeto estaba en la raíz, y null en `newParent` que pasa a la raíz.

- `oldParent`: padre anterior, o null.
- `newParent`: padre nuevo, o null.

## OnCollisionEnter

```cpp
virtual void OnCollisionEnter(const Physics::CollisionInfo& collision);
```

Primer frame de un contacto sólido con otro cuerpo. `collision.otherObject` es el otro `GameObject`, `contactPoint` y `contactNormal` describen el contacto, y `penetrationDepth` cuánto se solapan. Hace falta un `RigidBodyComponent` en este objeto; un collider suelto no entrega el mensaje. Si el otro no tiene objeto de escena, `otherObject` puede ser null.

- `collision`: datos del contacto de este frame.

## OnCollisionStay

```cpp
virtual void OnCollisionStay(const Physics::CollisionInfo& collision);
```

Se llama cada paso mientras el contacto sólido sigue activo, después del `OnCollisionEnter` y antes del `OnCollisionExit`.

- `collision`: datos del contacto de este paso.

## OnCollisionExit

```cpp
virtual void OnCollisionExit(const Physics::CollisionInfo& collision);
```

Se llama el frame en que el contacto sólido termina.

- `collision`: último contacto conocido.

## OnTriggerEnter

```cpp
virtual void OnTriggerEnter(const Physics::CollisionInfo& collision);
```

Igual que `OnCollisionEnter`, pero el collider de este objeto (o el otro) está marcado como trigger: no hay respuesta física, solo el aviso. Sin `RigidBodyComponent` en el objeto, Bullet no entrega el mensaje.

- `collision`: datos del solape.

## OnTriggerStay

```cpp
virtual void OnTriggerStay(const Physics::CollisionInfo& collision);
```

Se llama cada paso mientras el solape de trigger continúa.

- `collision`: datos del solape de este paso.

## OnTriggerExit

```cpp
virtual void OnTriggerExit(const Physics::CollisionInfo& collision);
```

Se llama cuando el solape de trigger termina.

- `collision`: último solape conocido.

## WantsTransparentRender

```cpp
virtual bool WantsTransparentRender() const;
```

Devuelve true si este componente quiere un pase de dibujo transparente propio. El valor por defecto es false y la escena no llama a `OnTransparentRender`. Un efecto de gameplay que pinte después de la geometría opaca lo pone a true.

**Devuelve** true si hay que llamar `OnTransparentRender` este frame.

## OnTransparentRender

```cpp
virtual void OnTransparentRender(Rendering::Camera* camera);
```

La escena lo llama durante el pase transparente solo si `WantsTransparentRender` devolvió true. `camera` es la cámara con la que se está dibujando la vista. El cuerpo por defecto no hace nada.

- `camera`: cámara del pase actual.

## WantsEditModeSimulate

```cpp
virtual bool WantsEditModeSimulate() const;
```

Devuelve true si el componente debe avanzar en la Scene View sin entrar en Play. El valor por defecto es false. Lo usan partículas y otros efectos con `simulateInEditMode`.

**Devuelve** true si la Scene View debe llamar `OnEditModeSimulate`.

## OnEditModeSimulate

```cpp
virtual void OnEditModeSimulate(float deltaTime);
```

La Scene View lo llama en Edit, sin partida, cuando `WantsEditModeSimulate` es true. No corren `OnUpdate` ni la física de juego.

- `deltaTime`: segundos desde la última simulación de edición.

## GetOwner

```cpp
GameObject* GetOwner() const;
```

Devuelve el `GameObject` al que está pegado este componente. Es null solo antes de que el motor asigne el dueño, o sea, no lo uses en el constructor.

**Devuelve** el dueño, o null si todavía no está asignado.

## SetEnabled

```cpp
void SetEnabled(bool enabled);
```

Activa o desactiva este componente sin tocar el `GameObject`. Al pasar a false se llama `OnDisable` si estaba activo en la jerarquía. Al pasar a true, y si el dueño está activo en la jerarquía, se llama `OnEnable` y, si aún no hubo `OnStart`, se encola. Si el valor no cambia, no hace nada.

- `enabled`: true para participar en el ciclo de vida.

## IsEnabled

```cpp
bool IsEnabled() const;
```

Lee la bandera propia del componente. No mira si el dueño está activo: para eso está `GameObject::IsActiveInHierarchy`.

**Devuelve** true si el componente está habilitado.

## SetUpdateTickEnabled

```cpp
void SetUpdateTickEnabled(bool enabled);
```

Enciende o apaga `OnUpdate`, `OnFixedUpdate` y `OnLateUpdate` sin deshabilitar el componente. `OnEnable` y `OnDisable` siguen ocurriendo. Útil para un objeto visible que no debe simularse.

- `enabled`: true para recibir los ticks.

## IsUpdateTickEnabled

```cpp
bool IsUpdateTickEnabled() const;
```

**Devuelve** true si los ticks de update están encendidos. El valor inicial es el de construcción del componente (encendido).

## SetTimeMode

```cpp
void SetTimeMode(ComponentTimeMode mode);
```

Elige si el tiempo de este componente y de sus `Invoke` usa la escala de tiempo del juego o el reloj sin escalar. `ComponentTimeMode::Scaled` es el default. `Unscaled` sigue corriendo cuando el tiempo de juego está a escala 0.

- `mode`: `Scaled` o `Unscaled`.

## GetTimeMode

```cpp
ComponentTimeMode GetTimeMode() const;
```

**Devuelve** el modo de tiempo actual. Por defecto es `Scaled`.

## Invoke

```cpp
Scripting::LatentActionHandle Invoke(float delaySeconds, std::function<void()> callback);
```

Programa `callback` para dentro de `delaySeconds` segundos, medidos con el `ComponentTimeMode` de este componente. Se cancela solo en `OnDestroy`. Un delay de 0 dispara en el siguiente avance del scheduler, no en mitad de la llamada.

- `delaySeconds`: espera antes de llamar.
- `callback`: función sin argumentos.

**Devuelve** un handle para `CancelInvoke`.

## InvokeRepeating

```cpp
Scripting::LatentActionHandle InvokeRepeating(float initialDelaySeconds, float intervalSeconds, std::function<void()> callback);
```

Igual que `Invoke`, y después repite `callback` cada `intervalSeconds`. También se cancela en `OnDestroy`.

- `initialDelaySeconds`: espera de la primera llamada.
- `intervalSeconds`: periodo entre llamadas.
- `callback`: función sin argumentos.

**Devuelve** un handle para `CancelInvoke`.

## StartSequence

```cpp
Scripting::LatentActionHandle StartSequence(Scripting::LatentSequence sequence);
```

Arranca una secuencia latente (esperas y pasos encadenados) en el scheduler del motor, con el mismo modo de tiempo que `Invoke`. Se cancela en `OnDestroy`.

- `sequence`: pasos a ejecutar.

**Devuelve** un handle para `CancelInvoke`.

## CancelInvoke

```cpp
void CancelInvoke(Scripting::LatentActionHandle handle);
```

Cancela la acción latente de ese handle. Un handle ya disparado o ya cancelado no hace nada.

- `handle`: valor devuelto por `Invoke`, `InvokeRepeating` o `StartSequence`.

## CancelAllInvokes

```cpp
void CancelAllInvokes();
```

Cancela todas las acciones latentes cuyo dueño es este componente. `OnDestroy` ya lo hace; llámalo antes si el componente sigue vivo y quieres vaciar la cola.

## GetTypeName

```cpp
virtual const char* GetTypeName() const = 0;
```

Nombre estable del tipo, el que usa el registro y el `.lua`. `RTB_COMPONENT` lo implementa. No lo escribas a mano.

**Devuelve** el nombre del tipo, por ejemplo `"Spinner"`.

## GetTypeInfo

```cpp
virtual const Reflection::TypeInfo* GetTypeInfo() const;
```

Metadatos de reflexión para el Inspector y el loader. `RTB_COMPONENT` lo rellena. Sin macro, devuelve null y el tipo no se serializa.

**Devuelve** el `TypeInfo` registrado, o null.
