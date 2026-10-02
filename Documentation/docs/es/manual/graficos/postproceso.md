# Postproceso

Después de la geometría el frame puede aplicar bloom, niebla de distancia y niebla volumétrica. Los valores de proyecto viven en `lighting.ini`. Los overrides locales viven en `VolumeComponent`.

## VolumeComponent

Un volumen es una caja en el mundo. Si la cámara está dentro (o en la franja de blend), sus efectos entran en la pila.

En el Inspector el componente no usa la lista genérica. El drawer propio enseña:

- Un checkbox de cabecera que activa el volumen entero
- **Zone**: global, tamaño de caja, distancia de blend, prioridad, peso
- **Distance Fog** y **Volumetric Fog**, cada uno con su checkbox

Esos checkboxes mapean a `overrideDistanceFog` y `overrideVolumetricFog`. Con el efecto apagado, sus parámetros no se aplican.

Un volumen **global** ignora la caja y afecta a toda la escena. La prioridad desempata cuando varios volúmenes solapan. El peso escala la contribución.

## Proyecto

**Window → Project Settings** edita ambiente, sombras, volumen DDGI, niebla de distancia y niebla volumétrica. **Save Project Settings** escribe `lighting.ini` y no cambia la API gráfica.

DDGI es iluminación global por probes. El volumen se configura en ese panel, no por GameObject. Cambiar la API gráfica (OpenGL / Vulkan) es otro ajuste y exige reiniciar el editor. Ver [OpenGL y Vulkan](rhi.md).

## Game View

El postproceso de la vista de juego es el del frame de la cámara principal. Los overlays del editor (grid, colliders, celdas de navegación) se dibujan solo en la Scene View, después de la geometría y antes de soltar el framebuffer.
