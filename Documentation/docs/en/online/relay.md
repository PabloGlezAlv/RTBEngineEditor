# Internet relay

`RTBOnlineRelay` is a .NET server. HTTP for the lobby, UDP for the match. Gameplay does not travel over HTTP.

| Port | Protocol | Use |
| --- | --- | --- |
| 8080 | TCP | `/api/v1` |
| 27100 | UDP | Relay. Datagrams up to 1200 bytes |

## API

Base path `/api/v1`. Errors are JSON `{ "error": "<code>", "message": "..." }`.

| Method | Path | Body |
| --- | --- | --- |
| GET | `/health` | — |
| POST | `/lobbies` | `displayName`, optional `maxMembers` from 2 to 6 |
| GET | `/lobbies/{code}` | — |
| POST | `/lobbies/{code}/join` | `displayName` |
| DELETE | `/lobbies/{code}` | Owner. `Authorization: Bearer {sessionToken}` |

Create and join return `lobbyId`, `sessionToken`, `memberId`, `isOwner`, `relay: { host, port }`, and `members`. Members of one lobby share the `sessionToken`.

## UDP

| Magic | Direction | Layout |
| --- | --- | --- |
| `RTBC` | Client → server | 4 bytes + 16 `sessionToken` + 16 `memberId`. Registers the endpoint. Repeat it if ~5 s pass with no traffic |
| `RTBG` | Client → server | 4 + 16 token + 16 `targetMemberId` + inner payload |
| `RTBG` | Server → client | 4 + 16 `senderMemberId` + inner payload |

`targetMemberId` set to `0xFF` repeated 16 times broadcasts to everyone except the sender. The inner payload is the game packet (`RTBR`, `RTBU`, `RTBA`).

## Run it locally

```powershell
cd RTBOnlineRelay
dotnet run
```

Or with Docker, after copying `.env.example` to `.env`:

```powershell
docker compose up -d --build
curl http://localhost:8080/api/v1/health
```

In the editor, **Window → Online**, URL `http://localhost:8080/api/v1`, Save And Apply. In game: Multiplayer → **Online Lobby**.

`RELAY_PUBLIC_HOST` must be an address the client can reach. On the same machine, `127.0.0.1`. On a VPS, the public IP or domain, with both ports open (`8080/tcp` and `27100/udp`). If the HTTP join works and the match does not, UDP is closed.

An HTTPS proxy can cover only the API. UDP still goes to `RELAY_PUBLIC_HOST:27100`.

Useful settings: `PublicHost`, `UdpPort` 27100, `LobbyTtlMinutes` 30, `MaxLobbies` 500.
