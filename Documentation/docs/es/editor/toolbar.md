# Reproducción

La barra superior controla el estado del editor y la compilación de scripts.

| Botón | Edit | Play | Pause |
| --- | --- | --- | --- |
| Play | Entra en Play | Desactivado | Reanuda |
| Pause | Desactivado | Pausa | El mismo botón reanuda |
| Stop | Desactivado | Vuelve a Edit y recarga | Igual |
| Compile Scripts | Compila | Desactivado | Desactivado |

No se compila en Play: la DLL está cargada.

Mientras MSBuild corre, un modal no cerrable muestra una barra indeterminada. Al acabar, si hubo error, el resultado queda en la consola (`MSBuildNotFound`, error de compilación o fallo al crear el proceso). La ruta de MSBuild que usa el editor es la de Visual Studio 2026 Community.

Compile Scripts lanza el build en otro hilo. El botón pasa a *Compiling...* hasta que ese hilo descarga la DLL vieja y carga la nueva.

Si Play no hace nada y estás en un prefab, es el bloqueo del modo prefab: vuelve a la escena con **Back to Scene**.
