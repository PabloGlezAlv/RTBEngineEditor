# Animator

`RTBEngine::Animation::Animator`. `LoadClipFromFbx` guarda un alias y `Play` usa ese alias. Las rutas son `Assets/...fbx`. Un alias o una ruta vacíos hacen que `LoadClipFromFbx` devuelva false.

```cpp
anim->LoadClipFromFbx("Run", "Assets/Characters/Knight/Run.fbx");
anim->Play("Run", true);
```

## ReloadClipLibrary

```cpp
void ReloadClipLibrary();
```

Vuelve a cargar esqueleto, mallas y clips que salen de `modelRef` y `additionalModels`. Los alias que metiste con `LoadClipFromFbx` y que no vienen de esos archivos se conservan. El clip en curso se suelta durante la recarga y, si seguía en reproducción, el animator intenta retomarlo por nombre.

## AddClip

```cpp
void AddClip(const std::string& name, std::shared_ptr<AnimationClip> clip);
```

Mete un clip ya construido en la biblioteca con ese nombre. `Play` lo encontrará por `name` después de `NormalizeClipName` (quita prefijos viejos tipo `mixamo.com|`).

- `name`: alias con el que luego llamas a `Play`.
- `clip`: clip en memoria. Un shared vacío no deja un clip reproducible.

## LoadClipFromFbx

```cpp
bool LoadClipFromFbx(const std::string& alias, const std::string& sourceFbx);
```

Carga clips del FBX y los guarda bajo `alias`. Si `sourceFbx` trae `ruta|nombreClip`, separa el archivo y el clip de dentro. Alias vacío o ruta vacía devuelven false sin cargar.

- `alias`: nombre para `Play`.
- `sourceFbx`: ruta `Assets/...fbx`, o `ruta|clip` para un clip concreto del archivo.

**Devuelve** true si el clip quedó en la biblioteca.

## ClearClips

```cpp
void ClearClips();
```

Vacía la biblioteca de clips. `Play` después de esto no encuentra nombres hasta que vuelvas a cargar.

## Play

```cpp
void Play(const std::string& clipName, bool loop = true);
```

Busca el clip por nombre normalizado, pone el tiempo a 0 y lo marca en reproducción. Si el nombre no está en la biblioteca, no cambia el clip actual. `loop` true (el default) repite al llegar al final; false se queda en el último frame según el avance del componente.

- `clipName`: alias de `LoadClipFromFbx` o de `AddClip`.
- `loop`: true para repetir. El default es true.

## Stop

```cpp
void Stop();
```

Corta la reproducción: `playing`, la pausa y la pose retenida pasan a false, el tiempo vuelve a 0 y el clip actual se olvida. Si hay esqueleto, los huesos vuelven a la bind pose. No borra la biblioteca: `Play` puede volver a elegir un alias.

## Pause

```cpp
void Pause();
```

Si `IsPlaying` es true, congela el tiempo del clip: `IsPaused` pasa a true y `IsPlaying` sigue true. Si no estaba en reproducción, no hace nada.

## Resume

```cpp
void Resume();
```

Si el clip sigue en reproducción (`IsPlaying`), quita la pausa y el tiempo sigue desde `GetCurrentTime`. Si `Stop` ya limpió el clip, no rearranca nada: hay que llamar a `Play`.

## HoldCurrentPose

```cpp
void HoldCurrentPose();
```

Para la reproducción y deja los huesos en el tiempo actual, recortado a la duración del clip. Sin esqueleto o sin clip actual, no hace nada. `ShouldSkinMesh` sigue true mientras la pose está retenida.

## IsPlaying

```cpp
bool IsPlaying() const;
```

**Devuelve** true si `Play` dejó el flag de reproducción. Una pausa no lo baja: mira `IsPaused`. El default es false.

## IsPaused

```cpp
bool IsPaused() const;
```

**Devuelve** true después de `Pause` y antes de `Resume` o de un `Play` nuevo.

## IsHoldingPose

```cpp
bool IsHoldingPose() const;
```

**Devuelve** true si `HoldCurrentPose` fijó la pose y nadie ha llamado a `Play` después.

## SetSpeed

```cpp
void SetSpeed(float spd);
```

Escala el avance del tiempo del clip. 1 es tiempo real, 0 lo congela sin pasar por `Pause`, 2 lo dobla. El default del campo es 1.

- `spd`: multiplicador del delta de animación.

## GetSpeed

```cpp
float GetSpeed() const;
```

**Devuelve** el multiplicador. El default es 1.

## GetCurrentTime

```cpp
float GetCurrentTime() const;
```

**Devuelve** los segundos dentro del clip actual. `Play` lo pone a 0. Sin clip sigue en 0.

## SelectClip

```cpp
void SelectClip(const std::string& clipName, bool loop = true);
```

Elige el clip y pone el tiempo a 0, pero no levanta el flag `playing`. Sirve para dejar un clip preparado. Si el nombre no existe, no cambia nada.

- `clipName`: alias normalizado.
- `loop`: valor que guardará `looping`. El default es true.

## ReloadKeyClips

```cpp
void ReloadKeyClips();
```

Vuelve a resolver la lista reflejada `keyClips` (clave, FBX, loop) a clips cargados. Llámalo si cambias `keyClips` en código y quieres que `PlayKey` vea las entradas nuevas.

## SetKeyClip

```cpp
bool SetKeyClip(const std::string& key, const std::string& clipFbxRef, bool loop);
```

Asocia una clave de gameplay (`"Attack"`) a un FBX y a si debe repetir, y recarga las claves. Si la clave ya existía, sustituye ruta y loop. Una clave vacía o un FBX vacío devuelven false y no tocan la lista.

- `key`: nombre que pasas a `PlayKey`. Vacío devuelve false.
- `clipFbxRef`: ruta del FBX, con el mismo convenio que `LoadClipFromFbx`. Vacía devuelve false.
- `loop`: si `PlayKey(key)` sin el bool debe repetir.

**Devuelve** true si la entrada quedó en `keyClips` y se pidió la recarga. La recarga puede fallar al leer el archivo; el bool de esta función no es el del load.

## HasKey

```cpp
bool HasKey(const std::string& key) const;
```

**Devuelve** true si `key` está en `keyClips`. Una clave vacía o desconocida devuelve false.

- `key`: nombre de la entrada.

## PlayKey

```cpp
bool PlayKey(const std::string& key);
```

Busca la clave y llama a `Play` con el alias normalizado y el loop guardado en la entrada. Si la clave no existe, devuelve false y no toca el clip actual.

- `key`: clave de `SetKeyClip` o del Inspector.

**Devuelve** true si después de `Play` hay un clip actual.

## PlayKey

```cpp
bool PlayKey(const std::string& key, bool loop);
```

Igual que `PlayKey(key)`, pero el `loop` de esta llamada manda sobre el de la entrada. Clave desconocida: false, sin cambiar el clip.

- `key`: clave.
- `loop`: si este disparo repite.

**Devuelve** true si quedó un clip en reproducción.

## IsPlayingKey

```cpp
bool IsPlayingKey(const std::string& key) const;
```

**Devuelve** true si `IsPlaying` es true y el nombre del clip en curso, ya normalizado, es esa clave. Si no está en reproducción, o la clave está vacía, devuelve false. Una pausa no lo apaga: `IsPlaying` sigue true.

- `key`: clave a comprobar.

## NotifyKeyFinished

```cpp
void NotifyKeyFinished(const std::string& key);
```

El animator lo llama al terminar un clip de clave que no hace loop, y dispara el evento de clave terminada. Está en la parte privada de la clase: un script no lo llama. Para enterarte, el componente ya publica el evento; no hace falta invocar este método.

- `key`: clave que se considera terminada.

## HasBones

```cpp
bool HasBones() const;
```

**Devuelve** true si hay esqueleto y al menos un hueso. Sin FBX cargado es false.

## ShouldSkinMesh

```cpp
bool ShouldSkinMesh() const;
```

**Devuelve** true si hay huesos, las matrices finales no están vacías, y el animator está en `Play` o reteniendo la pose. El renderer lo usa para decidir si sube el skinning. Parado y sin `HoldCurrentPose`, es false.

## CreateBoneGameObjects

```cpp
void CreateBoneGameObjects(Scene::Scene* scene);
```

Crea un `GameObject` por hueso y lo marca con `SetAnimatorBone`. Sin escena, no tiene dónde insertarlos. Llamarlo dos veces no debe duplicar si `AreBoneGOsCreated` ya es true: compruébalo antes.

- `scene`: escena que poseerá los huesos. Null no crea objetos.

## SyncBoneGameObjects

```cpp
void SyncBoneGameObjects();
```

Copia la pose actual del esqueleto a los `GameObject` de hueso. Sin huesos creados, no tiene transforms que mover. El animator ya lo hace en su actualización cuando los huesos existen; llámalo si acabas de crearlos y quieres la pose de este frame.

## AreBoneGOsCreated

```cpp
bool AreBoneGOsCreated() const;
```

**Devuelve** true después de un `CreateBoneGameObjects` que llegó a crear la lista. El default es false.

## IsBoneGameObject

```cpp
bool IsBoneGameObject(const Scene::GameObject* go) const;
```

**Devuelve** true si `go` es uno de los huesos de este animator. Null devuelve false.

- `go`: objeto a comprobar.
