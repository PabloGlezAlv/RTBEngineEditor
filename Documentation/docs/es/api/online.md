# Online

`RTBEngine::Online::OnlineSystem::GetInstance()`. `Initialize` lo hace el editor si el panel Online está enabled, o el player al arrancar con red. Un script de menú no vuelve a inicializar el sistema: elige backend, nombre y usa el lobby.

Los datagramas del relay (`RTBC`, `RTBG`) los escribe el transporte, no el script. Ver [Relay de Internet](../online/relay.md).

```cpp
auto& net = RTBEngine::Online::OnlineSystem::GetInstance();
if (net.IsInLobby() && net.IsLobbyOwner()) {
    StartMatch();
}
```

## Initialize

```cpp
bool Initialize(const OnlineConfig& config);
```

Apaga lo que hubiera y arranca con esa config: `enabled`, backend por defecto y nombre de login. Si `enabled` es false, deja el estado en `Disabled` y devuelve true: no es un fallo. Si el backend no arranca, el estado pasa a `Error`, `GetLastError` tiene el texto, y el bool es false solo cuando `failApplicationOnError` es true. Llamarlo otra vez tira la sesión actual porque empieza por `Shutdown`.

- `config`: opciones de arranque.

**Devuelve** true si quedó deshabilitado a propósito, si inicializó, o si falló pero la config no pide tumbar la aplicación. False si falló y `failApplicationOnError` es true.

## Tick

```cpp
void Tick(float deltaTime);
```

Avanza lobby y transporte, y bombea la red de gameplay. Si el estado no es `Initialized`, vuelve sin hacer nada. Lo llama el bucle de la aplicación.

- `deltaTime`: segundos del frame.

## Shutdown

```cpp
void Shutdown();
```

Cierra backends y deja el estado en apagado. `Initialize` lo llama al empezar. El player lo llama al salir. Desde un script de partida no lo uses para “salir del lobby”: eso es el `IOnlineLobby`.

## IsEnabled

```cpp
bool IsEnabled() const;
```

**Devuelve** el flag `enabled` de la última config. El default, antes de `Initialize`, es false.

## IsInitialized

```cpp
bool IsInitialized() const;
```

**Devuelve** true solo cuando el estado es `OnlineState::Initialized`. Apagado, arrancando o en error es false.

## GetState

```cpp
OnlineState GetState() const;
```

**Devuelve** el estado. Antes de inicializar es `Disabled`.

## GetLastError

```cpp
const std::string& GetLastError() const;
```

**Devuelve** el último error guardado. Vacío si no hubo fallo. No limpies la cadena: es del motor.

## SetSessionDisplayName

```cpp
void SetSessionDisplayName(const std::string& name);
```

Nombre que esta sesión enseña en el lobby. Una cadena vacía deja el nombre vacío.

- `name`: nombre visible.

## GetSessionDisplayName

```cpp
const std::string& GetSessionDisplayName() const;
```

**Devuelve** el nombre de sesión. Vacío si nadie lo puso.

## SetSessionLobbyBackend

```cpp
void SetSessionLobbyBackend(OnlineBackendType backend);
```

Elige qué lobby usa la UI de esta sesión, LAN o relay. No apaga el otro: las dos pilas siguen inicializadas. `GetLobby()` sin argumentos pasa a mirar este backend cuando no hay un lobby ya ocupado en el otro.

- `backend`: `OnlineBackendType` de LAN o de relay.

## ClearSessionLobbyBackend

```cpp
void ClearSessionLobbyBackend();
```

Quita la preferencia. `HasSessionLobbyBackend` pasa a false y `GetLobby()` vuelve a resolver el backend activo por quién está de verdad en un lobby.

## HasSessionLobbyBackend

```cpp
bool HasSessionLobbyBackend() const;
```

**Devuelve** true después de `SetSessionLobbyBackend` y antes de `ClearSessionLobbyBackend`. El default es false.

## GetSessionLobbyBackend

```cpp
OnlineBackendType GetSessionLobbyBackend() const;
```

**Devuelve** el backend elegido. Si nunca se eligió, el campo interno arranca en LAN, pero `HasSessionLobbyBackend` es false y no debes tratarlo como una elección del jugador.

## IsInLobby

```cpp
bool IsInLobby() const;
```

**Devuelve** true si el lobby activo existe y su id no está vacío. Sin backend o sin sala, false.

## IsLobbyOwner

```cpp
bool IsLobbyOwner() const;
```

**Devuelve** true si estás en un lobby y la ficha de esa sala te marca como dueño. Sin sala, false. El cliente que se unió recibe false.

## GetLocalUserId

```cpp
OnlineUserId GetLocalUserId() const;
```

**Devuelve** el id local de la identidad. Sin identidad inicializada devuelve un `OnlineUserId` vacío (`IsValid()` false).

## GetOrderedLobbyMembers

```cpp
std::vector<OnlineUserId> GetOrderedLobbyMembers() const;
```

Miembros de la sala, el dueño primero y después el resto, sin duplicar al dueño. Sin lobby devuelve un vector vacío. El índice en esta lista es el slot de jugador que usa `NetworkIdentity::SetNetworkPlayerSlot`.

**Devuelve** la lista ordenada, o vacía.

## GetLocalPlayerIndex

```cpp
std::size_t GetLocalPlayerIndex() const;
```

Posición del usuario local dentro de `GetOrderedLobbyMembers`. Si el id local no está en la lista, devuelve 0: ese 0 no significa “eres el dueño” cuando la lista está vacía.

**Devuelve** el índice, o 0 si no aparece.

## GetLobbyMemberDisplayName

```cpp
std::string GetLobbyMemberDisplayName(const OnlineUserId& member) const;
```

Nombre visible de ese miembro. Un id que no está en la sala devuelve una cadena vacía.

- `member`: id de `GetOrderedLobbyMembers` o `GetLocalUserId`.

**Devuelve** el nombre, o vacío.

## SetPlayerSessionProfile

```cpp
void SetPlayerSessionProfile(const OnlinePlayerProfile& profile);
```

Guarda el perfil de sesión para la partida que va a empezar. Un `playerSlot` negativo o un `displayName` vacío no hacen nada y no sustituyen un perfil ya guardado. Si los dos son válidos, reemplaza el perfil de ese slot.

- `profile`: datos del jugador. Slot negativo o nombre vacío: la llamada se ignora.

## RemovePlayerSessionProfile

```cpp
void RemovePlayerSessionProfile(int playerSlot);
```

Quita el perfil de ese slot y publica el evento de perfil quitado. Un slot negativo vuelve sin borrar nada. Un slot que no estaba en el mapa tampoco borra una entrada, pero el evento sí se publica.

- `playerSlot`: slot. Negativo: no hace nada.

## TryGetPlayerSessionProfile

```cpp
bool TryGetPlayerSessionProfile(int playerSlot, OnlinePlayerProfile& outProfile) const;
```

Copia el perfil a `outProfile` si ese slot existe.

- `playerSlot`: slot.
- `outProfile`: salida. Si devuelve false, no la uses.

**Devuelve** true si había perfil.

## GetPlayerDisplayName

```cpp
std::string GetPlayerDisplayName(int playerSlot) const;
```

**Devuelve** el nombre del perfil de ese slot, o vacío si no hay perfil.

- `playerSlot`: slot.

## GetLobby

```cpp
IOnlineLobby* GetLobby();
```

El lobby del backend de sesión, o el que esté de verdad en una sala si no fijaste preferencia. Null si el sistema no está inicializado.

**Devuelve** el lobby, o null.

## GetLobby

```cpp
IOnlineLobby* GetLobby(OnlineBackendType backend);
```

El lobby de ese backend concreto, aunque la preferencia de sesión sea la otra. Null si esa pila no está lista.

- `backend`: LAN o relay.

**Devuelve** el lobby, o null.

## GetTransport

```cpp
IOnlineTransport* GetTransport();
```

El transporte por el que viajan los mensajes de partida. Null antes de `Initialize`. Los scripts de juego envían por aquí; no escriben `RTBC` ni `RTBG`.

**Devuelve** el transporte, o null.

## GetIdentity

```cpp
IOnlineIdentity* GetIdentity();
```

La identidad local (usuario, login). Null antes de `Initialize`.

**Devuelve** la identidad, o null.
