# Mensajes de red

El protocolo de gameplay del motor es **RTBN v4** (`OnlineMessageCodec.h`). Por encima, el juego define sus propios ids en `Assets/Scripts` (`GameNetMessageIds.h`). Esos ids de juego empiezan en 64 para no pisar los del motor.

## Modelo

La partida es **autoritativa en el host**. El cliente manda intención. El host simula y replica estado.

| Componente | Rol |
| --- | --- |
| `NetworkIdentity` | Id de red, usuario dueño, slot de jugador |
| `NetworkTransform` | Replica la pose del objeto |
| `OnlineSystem` | Lobby, identidad local, perfiles de sesión, transporte |
| `OnlinePlayerManager` | Spawnea y quita peones remotos |

```cpp
auto& net = RTBEngine::Online::OnlineSystem::GetInstance();
net.SetSessionDisplayName("Ada");
net.SetSessionLobbyBackend(RTBEngine::Online::OnlineBackendType::Lan);
bool owner = net.IsLobbyOwner();
```

`SetSessionLobbyBackend` solo elige el lobby de esta sesión. No apaga el otro backend.

## Slots

`GetOrderedLobbyMembers` da el orden. `GetLocalPlayerIndex` es el índice local. `SetPlayerSessionProfile` / `TryGetPlayerSessionProfile` guardan el nombre visible por slot. `SubscribeToPlayerSessionProfileChanged` avisa cuando un perfil entra o sale (`removed`).

`NetworkIdentity::SetNetworkPlayerSlot` ata el peón a ese slot. El manager tiene que correr su `OnStart` antes que el del jugador: ordena el GameObject del manager antes que el del peón en la escena.

## Salir de la partida

El componente de mensajes del juego avisa al host. El host despawnea y reenvía el slot que se fue. Si se cae el host, los clientes reciben el abandono. Un corte sin mensaje se cubre comparando la lista de miembros del lobby con los peones vivos; el relay actualiza esa lista antes que el descubrimiento LAN.

## Después de cambiar la API online

Regenera el SDK (`BuildSDK.bat`) y recompila `GameScripts`. Un editor con SDK viejo y scripts nuevos no registra los componentes de red y la consola se llena de errores al cargar la escena.
