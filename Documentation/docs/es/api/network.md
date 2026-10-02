# Network

Componentes de escena para una partida ya en curso. El lobby se hace con [Online](online.md).

## SetOwnerUserId

```cpp
void NetworkIdentity::SetOwnerUserId(const Online::OnlineUserId& userId);
```

Ata este `GameObject` al miembro. Si `userId.IsValid()` es false, guarda una cadena vacía: el peón queda sin dueño explícito. El manager de jugadores debe recibir `OnStart` antes que el peón, para que el slot y el id ya estén escritos cuando el peón arranca.

- `userId`: id de `OnlineSystem::GetLocalUserId` o de `GetOrderedLobbyMembers`.

## SetNetworkPlayerSlot

```cpp
void NetworkIdentity::SetNetworkPlayerSlot(int slot);
```

Slot de este peón en `GetOrderedLobbyMembers`. El local compara este número con `GetLocalPlayerIndex`. El campo arranca en `-1`, que significa “sin slot”. Un slot que no está en la lista no falla aquí: la comprobación de “¿es el mío?” simplemente no coincidirá.

- `slot`: índice en la lista ordenada, o `-1` para ninguno.

## SetNetworkId

```cpp
void NetworkIdentity::SetNetworkId(std::uint32_t id);
```

Identidad de red del objeto para los snapshots. `0` significa “sin id” (`HasNetworkId` es false). El host asigna ids distintos de cero; un cliente no inventa el id de un peón remoto.

- `id`: identificador. `0` lo deja sin id.

## NetworkTransform

Replica la pose. Colócalo en el mismo objeto que la identidad. El host escribe la pose autoritativa; los clientes aplican la replicada en el peón remoto.

No teletransportes un peón remoto desde el cliente: el siguiente snapshot del host lo pisa. El input local se envía como mensaje de juego (ids a partir de 64 en `GameNetMessageIds.h`) y el host mueve el cuerpo.

RTBN v4 es el codec del motor. Los mensajes de partida viven en `GameScripts` y viajan por `IOnlineTransport`. El relay los mete dentro de `RTBG` sin interpretarlos. Si cambias el layout de un mensaje, sube el acuerdo entre host y cliente y recompila todos los `GameScripts.dll` que vayan a jugar juntos.
