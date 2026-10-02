# LAN match

The LAN lobby discovers over UDP, and the match uses another UDP port. Each instance on the same machine needs different ports.

| Instance | Game port | Discovery port |
| --- | --- | --- |
| Editor / player 1 | 27015 | 27016 |
| Second instance or Multiplayer Test | 27017 | 27018 |

Edit those values in **Window → Online**. **Multiplayer Test** launches another instance with the ports offset.

## Try it

1. Compile `GameScripts` if you touched scripts.
2. Play `MainMenu.lua`.
3. Multiplayer → **LAN Lobby**.
4. On the other instance, enter the lobby with the code or via discovery.
5. The host presses **Start Game** when at least one remote member is connected.

In `DefaultScene.lua`, the player-manager object must be listed **before** the pawn. That way the manager's `OnStart` configures `NetworkIdentity` before the player's first tick.

## Leaving

| Who leaves | What the others see |
| --- | --- |
| A client, from the pause menu | The host removes that pawn and tells the rest. They despawn it |
| The host | Clients return to the menu with a notice that the host abandoned the match |
| A hard close | The host notices the member is gone from the lobby and despawns |

Each player should see their own camera on their pawn, and the remote movement on the other. If you only see the lobby and not the movement, check that the game ports do not collide and that the console has no `GameScripts` registration errors.
