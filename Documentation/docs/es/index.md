# RTBEngine

Motor 3D en C++17 para Windows, con editor visual al estilo de un motor de componentes. Esta web es el manual, la guía del editor y la referencia de scripting.

<div class="grid cards" markdown>

-   :material-book-open-page-variant:{ .lg .middle } **Manual**

    ---

    Arquitectura, escena, scripting, física, gráficos, UI y ECS.

    [:octicons-arrow-right-24: Abrir el manual](manual/index.md)

-   :material-view-dashboard:{ .lg .middle } **Editor**

    ---

    Jerarquía, Inspector, vistas, prefabs, atajos y build del juego.

    [:octicons-arrow-right-24: Guía del editor](editor/index.md)

-   :material-lan:{ .lg .middle } **Online**

    ---

    Lobby LAN, relay HTTP/UDP y componentes de red.

    [:octicons-arrow-right-24: Multijugador](online/index.md)

-   :material-code-braces:{ .lg .middle } **Scripting API**

    ---

    Clases, métodos y ejemplos de `GameScripts`.

    [:octicons-arrow-right-24: Referencia](api/index.md)

</div>

## Tres piezas

| Pieza | Qué es | Versión |
| --- | --- | --- |
| `RTBEngine.dll` | Motor: escena, render, física, audio, red | 1.0.0 |
| `RTBEngineEditor` | Editor ImGui que consume el SDK | 1.0.0 |
| `RTBOnlineRelay` | Matchmaking HTTP y relay UDP | API `/api/v1` |

El autorado vive en `RTBEngine::Scene` (GameObject y Component). La simulación densa vive en `RTBEngine::ECS`. El juego no reimplementa el motor: registra componentes en `GameScripts.dll`.

## Por dónde seguir

1. [Qué es RTBEngine](manual/que-es.md) si quieres el mapa del proyecto.
2. [Instalación](manual/instalacion.md) si vas a compilar motor y editor.
3. [Tu primer juego](manual/primer-juego.md) si ya tienes el editor abierto.
4. [Componentes C++](manual/scripting/componentes-cpp.md) para el primer script.

!!! note "Idioma"
    Español es el idioma por defecto. English lives under the language selector, at `/en/`.
