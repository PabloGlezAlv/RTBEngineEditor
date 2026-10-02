# Partida en LAN

El lobby LAN descubre por UDP y la partida va por otro puerto UDP. Cada instancia de la misma máquina necesita puertos distintos.

| Instancia | Puerto de juego | Puerto de discovery |
| --- | --- | --- |
| Editor / jugador 1 | 27015 | 27016 |
| Segunda instancia o Multiplayer Test | 27017 | 27018 |

Esos valores se editan en **Window → Online**. **Multiplayer Test** lanza otra instancia con los puertos desplazados.

## Probar

1. Compila `GameScripts` si tocaste scripts.
2. Play en `MainMenu.lua`.
3. Multiplayer → **LAN Lobby**.
4. En la otra instancia, entra al lobby con el código o el descubrimiento.
5. El host pulsa **Start Game** cuando hay al menos un miembro remoto.

En `DefaultScene.lua`, el objeto del manager de jugadores tiene que aparecer **antes** que el peón. Así `OnStart` del manager configura la `NetworkIdentity` antes del primer tick del jugador.

## Salir

| Quién sale | Qué ven los demás |
| --- | --- |
| Un cliente, por el menú de pausa | El host quita su peón y avisa. El resto lo despawnea |
| El host | Los clientes vuelven al menú con el aviso de que el host abandonó |
| Cierre brusco | El host detecta que el miembro ya no está en el lobby y despawnea |

Cada jugador debe ver su propia cámara en su peón, y el movimiento remoto en el otro. Si solo se ve el lobby y no el movimiento, revisa que los puertos de juego no coincidan y que la consola no tenga errores de registro de `GameScripts`.
