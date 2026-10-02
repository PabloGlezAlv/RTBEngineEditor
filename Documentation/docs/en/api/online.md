# Online

`RTBEngine::Online::OnlineSystem::GetInstance()`. The editor calls `Initialize` when the Online panel is enabled, or the player does it when starting with networking. A menu script does not initialize the system again: it picks a backend, a name, and uses the lobby.

The relay datagrams (`RTBC`, `RTBG`) are written by the transport, not by the script. See [Internet relay](../online/relay.md).

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

Shuts down whatever was running and starts with that config: `enabled`, the default backend, and the login name. If `enabled` is false, it leaves the state at `Disabled` and returns true: that is not a failure. If the backend fails to start, the state becomes `Error`, `GetLastError` has the text, and the bool is false only when `failApplicationOnError` is true. Calling it again drops the current session because it starts with `Shutdown`.

- `config`: startup options.

**Returns** true if it was left disabled on purpose, if it initialized, or if it failed but the config does not ask to take the application down. False if it failed and `failApplicationOnError` is true.

## Tick

```cpp
void Tick(float deltaTime);
```

Advances the lobby and the transport, and pumps gameplay net. If the state is not `Initialized`, it returns without doing anything. The application loop calls it.

- `deltaTime`: seconds of the frame.

## Shutdown

```cpp
void Shutdown();
```

Closes backends and leaves the state off. `Initialize` calls it at the start. The player calls it on exit. From a match script, do not use it to “leave the lobby”: that is `IOnlineLobby`.

## IsEnabled

```cpp
bool IsEnabled() const;
```

**Returns** the `enabled` flag from the last config. The default, before `Initialize`, is false.

## IsInitialized

```cpp
bool IsInitialized() const;
```

**Returns** true only when the state is `OnlineState::Initialized`. Off, starting, or failed is false.

## GetState

```cpp
OnlineState GetState() const;
```

**Returns** the state. Before initialization it is `Disabled`.

## GetLastError

```cpp
const std::string& GetLastError() const;
```

**Returns** the last stored error. Empty if there was no failure. Do not clear the string: it belongs to the engine.

## SetSessionDisplayName

```cpp
void SetSessionDisplayName(const std::string& name);
```

Name this session shows in the lobby. An empty string stores an empty name.

- `name`: visible name.

## GetSessionDisplayName

```cpp
const std::string& GetSessionDisplayName() const;
```

**Returns** the session name. Empty if nobody set it.

## SetSessionLobbyBackend

```cpp
void SetSessionLobbyBackend(OnlineBackendType backend);
```

Chooses which lobby this session's UI uses, LAN or relay. It does not shut the other one down: both stacks stay initialized. `GetLobby()` with no arguments then looks at this backend when the other one is not already in a lobby.

- `backend`: the LAN or relay `OnlineBackendType`.

## ClearSessionLobbyBackend

```cpp
void ClearSessionLobbyBackend();
```

Clears the preference. `HasSessionLobbyBackend` becomes false and `GetLobby()` resolves the active backend from whoever is actually in a lobby.

## HasSessionLobbyBackend

```cpp
bool HasSessionLobbyBackend() const;
```

**Returns** true after `SetSessionLobbyBackend` and before `ClearSessionLobbyBackend`. The default is false.

## GetSessionLobbyBackend

```cpp
OnlineBackendType GetSessionLobbyBackend() const;
```

**Returns** the chosen backend. If it was never chosen, the internal field starts at LAN, but `HasSessionLobbyBackend` is false and you should not treat it as the player's choice.

## IsInLobby

```cpp
bool IsInLobby() const;
```

**Returns** true if the active lobby exists and its id is not empty. With no backend or no room, false.

## IsLobbyOwner

```cpp
bool IsLobbyOwner() const;
```

**Returns** true if you are in a lobby and that room's record marks you as owner. With no room, false. A client that joined gets false.

## GetLocalUserId

```cpp
OnlineUserId GetLocalUserId() const;
```

**Returns** the local identity id. With no initialized identity it returns an empty `OnlineUserId` (`IsValid()` false).

## GetOrderedLobbyMembers

```cpp
std::vector<OnlineUserId> GetOrderedLobbyMembers() const;
```

Room members, owner first and then the rest, without duplicating the owner. With no lobby it returns an empty vector. The index in this list is the player slot `NetworkIdentity::SetNetworkPlayerSlot` uses.

**Returns** the ordered list, or empty.

## GetLocalPlayerIndex

```cpp
std::size_t GetLocalPlayerIndex() const;
```

Position of the local user inside `GetOrderedLobbyMembers`. If the local id is not in the list, it returns 0: that 0 does not mean “you are the owner” when the list is empty.

**Returns** the index, or 0 if the user is not in the list.

## GetLobbyMemberDisplayName

```cpp
std::string GetLobbyMemberDisplayName(const OnlineUserId& member) const;
```

Visible name of that member. An id that is not in the room returns an empty string.

- `member`: an id from `GetOrderedLobbyMembers` or `GetLocalUserId`.

**Returns** the name, or empty.

## SetPlayerSessionProfile

```cpp
void SetPlayerSessionProfile(const OnlinePlayerProfile& profile);
```

Stores the session profile for the match that is about to start. A negative `playerSlot` or an empty `displayName` does nothing and does not replace a profile already stored. If both are valid, it replaces the profile on that slot.

- `profile`: player data. A negative slot or an empty name: the call is ignored.

## RemovePlayerSessionProfile

```cpp
void RemovePlayerSessionProfile(int playerSlot);
```

Removes the profile for that slot and publishes the profile-removed event. A negative slot returns without erasing anything. A slot that was not in the map does not erase an entry, but the event is still published.

- `playerSlot`: slot. Negative: nothing happens.

## TryGetPlayerSessionProfile

```cpp
bool TryGetPlayerSessionProfile(int playerSlot, OnlinePlayerProfile& outProfile) const;
```

Copies the profile into `outProfile` if that slot exists.

- `playerSlot`: slot.
- `outProfile`: output. If this returns false, do not use it.

**Returns** true if a profile was stored.

## GetPlayerDisplayName

```cpp
std::string GetPlayerDisplayName(int playerSlot) const;
```

**Returns** the profile name for that slot, or empty if there is no profile.

- `playerSlot`: slot.

## GetLobby

```cpp
IOnlineLobby* GetLobby();
```

The lobby for the session backend, or the one that is actually in a room if you did not set a preference. Null if the system is not initialized.

**Returns** the lobby, or null.

## GetLobby

```cpp
IOnlineLobby* GetLobby(OnlineBackendType backend);
```

The lobby for that specific backend, even when the session preference is the other one. Null if that stack is not ready.

- `backend`: LAN or relay.

**Returns** the lobby, or null.

## GetTransport

```cpp
IOnlineTransport* GetTransport();
```

The transport match messages travel on. Null before `Initialize`. Game scripts send through here; they do not write `RTBC` or `RTBG`.

**Returns** the transport, or null.

## GetIdentity

```cpp
IOnlineIdentity* GetIdentity();
```

The local identity (user, login). Null before `Initialize`.

**Returns** the identity, or null.
