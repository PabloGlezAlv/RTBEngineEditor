# Network messages

The engine gameplay protocol is **RTBN v4** (`OnlineMessageCodec.h`). Above that, the game defines its own ids in `Assets/Scripts` (`GameNetMessageIds.h`). Those game ids start at 64 so they do not collide with the engine ids.

## Model

The match is **host-authoritative**. The client sends intent. The host simulates and replicates state.

| Component | Role |
| --- | --- |
| `NetworkIdentity` | Network id, owning user, player slot |
| `NetworkTransform` | Replicates the object's pose |
| `OnlineSystem` | Lobby, local identity, session profiles, transport |
| `OnlinePlayerManager` | Spawns and removes remote pawns |

```cpp
auto& net = RTBEngine::Online::OnlineSystem::GetInstance();
net.SetSessionDisplayName("Ada");
net.SetSessionLobbyBackend(RTBEngine::Online::OnlineBackendType::Lan);
bool owner = net.IsLobbyOwner();
```

`SetSessionLobbyBackend` only picks this session's lobby. It does not shut the other backend down.

## Slots

`GetOrderedLobbyMembers` returns the order. `GetLocalPlayerIndex` is the local index. `SetPlayerSessionProfile` / `TryGetPlayerSessionProfile` store the display name per slot. `SubscribeToPlayerSessionProfileChanged` fires when a profile joins or leaves (`removed`).

`NetworkIdentity::SetNetworkPlayerSlot` binds the pawn to that slot. The manager must run `OnStart` before the player: place the manager GameObject before the pawn in the scene.

## Leaving the match

The game message component tells the host. The host despawns and forwards the slot that left. If the host drops, clients receive the abandon notice. A cut with no message is covered by comparing the lobby member list with the living pawns; the relay updates that list sooner than LAN discovery.

## After an online API change

Regenerate the SDK (`BuildSDK.bat`) and rebuild `GameScripts`. An editor with an old SDK and new scripts does not register the network components, and the console fills with errors when the scene loads.
