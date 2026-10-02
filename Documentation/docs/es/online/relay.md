# Relay de Internet

`RTBOnlineRelay` es un servidor .NET. HTTP para el lobby, UDP para la partida. El gameplay no viaja por HTTP.

| Puerto | Protocolo | Uso |
| --- | --- | --- |
| 8080 | TCP | `/api/v1` |
| 27100 | UDP | Relay. Datagramas de hasta 1200 bytes |

## API

Base `/api/v1`. Los errores son JSON `{ "error": "<code>", "message": "..." }`.

| Método | Ruta | Cuerpo |
| --- | --- | --- |
| GET | `/health` | — |
| POST | `/lobbies` | `displayName`, `maxMembers` opcional de 2 a 6 |
| GET | `/lobbies/{code}` | — |
| POST | `/lobbies/{code}/join` | `displayName` |
| DELETE | `/lobbies/{code}` | Dueño. `Authorization: Bearer {sessionToken}` |

Crear y unir devuelven `lobbyId`, `sessionToken`, `memberId`, `isOwner`, `relay: { host, port }` y `members`. Los miembros de un lobby comparten el `sessionToken`.

## UDP

| Magic | Dirección | Layout |
| --- | --- | --- |
| `RTBC` | Cliente → servidor | 4 bytes + 16 `sessionToken` + 16 `memberId`. Registra el extremo. Repítelo si pasan ~5 s sin tráfico |
| `RTBG` | Cliente → servidor | 4 + 16 token + 16 `targetMemberId` + payload interno |
| `RTBG` | Servidor → cliente | 4 + 16 `senderMemberId` + payload interno |

`targetMemberId` a `0xFF` repetido 16 veces es broadcast a todos menos al emisor. El payload interno es el paquete de juego (`RTBR`, `RTBU`, `RTBA`).

## Arrancar en local

```powershell
cd RTBOnlineRelay
dotnet run
```

O con Docker, tras copiar `.env.example` a `.env`:

```powershell
docker compose up -d --build
curl http://localhost:8080/api/v1/health
```

En el editor, **Window → Online**, URL `http://localhost:8080/api/v1`, Save And Apply. En el juego: Multiplayer → **Online Lobby**.

`RELAY_PUBLIC_HOST` tiene que ser una dirección que el cliente pueda usar. En la misma máquina, `127.0.0.1`. En un VPS, la IP pública o el dominio, y los dos puertos abiertos (`8080/tcp` y `27100/udp`). Si el join HTTP funciona y la partida no, el UDP está cerrado.

Un proxy HTTPS puede tapar solo el API. El UDP sigue yendo a `RELAY_PUBLIC_HOST:27100`.

Configuración útil: `PublicHost`, `UdpPort` 27100, `LobbyTtlMinutes` 30, `MaxLobbies` 500.
