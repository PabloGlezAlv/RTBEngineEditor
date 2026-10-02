# Network

Scene components for a match that is already running. The lobby is [Online](online.md).

## SetOwnerUserId

```cpp
void NetworkIdentity::SetOwnerUserId(const Online::OnlineUserId& userId);
```

Binds this `GameObject` to the member. If `userId.IsValid()` is false, it stores an empty string: the pawn has no explicit owner. The player manager must receive `OnStart` before the pawn, so the slot and the id are already written when the pawn starts.

- `userId`: an id from `OnlineSystem::GetLocalUserId` or `GetOrderedLobbyMembers`.

## SetNetworkPlayerSlot

```cpp
void NetworkIdentity::SetNetworkPlayerSlot(int slot);
```

This pawn's slot in `GetOrderedLobbyMembers`. The local side compares this number with `GetLocalPlayerIndex`. The field starts at `-1`, which means “no slot”. A slot that is not in the list does not fail here: the “is this mine?” check simply will not match.

- `slot`: index in the ordered list, or `-1` for none.

## SetNetworkId

```cpp
void NetworkIdentity::SetNetworkId(std::uint32_t id);
```

Network identity of the object for snapshots. `0` means “no id” (`HasNetworkId` is false). The host assigns ids other than zero; a client does not invent the id of a remote pawn.

- `id`: identifier. `0` leaves it without an id.

## NetworkTransform

Replicates the pose. Put it on the same object as the identity. The host writes the authoritative pose; clients apply the replicated one on the remote pawn.

Do not teleport a remote pawn from the client: the host's next snapshot overwrites it. Local input is sent as a game message (ids from 64 in `GameNetMessageIds.h`) and the host moves the body.

RTBN v4 is the engine codec. Match messages live in `GameScripts` and travel on `IOnlineTransport`. The relay wraps them in `RTBG` without interpreting them. If you change a message layout, bump the agreement between host and client and rebuild every `GameScripts.dll` that will play together.
