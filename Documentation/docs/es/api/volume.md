# Volume

`RTBEngine::Scene::VolumeComponent` mete niebla de distancia y niebla volumétrica en la pila de postproceso mientras la cámara está en la zona, o siempre si el volumen es global.

El Inspector es propio:

| Bloque | Campos |
| --- | --- |
| Cabecera | Activa o apaga el volumen en la pila |
| Zone | Global, tamaño de caja, blend, prioridad, peso |
| Distance Fog | `overrideDistanceFog` y parámetros |
| Volumetric Fog | `overrideVolumetricFog` y parámetros |
| Bloom | `overrideBloom`, `bloomEnabled`, `bloomThreshold`, `bloomIntensity` |

Si el checkbox del efecto está apagado, ese efecto no entra en el blend aunque el volumen esté activo.

La niebla de proyecto, sin volumen, se edita en `lighting.ini` desde Project Settings. El volumen solo añade overrides locales. Dos volúmenes solapados se ordenan por prioridad y se escalan por peso.

No hace falta llamarlo desde un script para que funcione: colócalo en la escena y ajusta la caja en el transform y en Zone.

## ToProfile

```cpp
Rendering::VolumeProfile ToProfile() const;
```

Copia los overrides de este componente a un `VolumeProfile`: niebla de distancia, niebla volumétrica y bloom, cada uno con su flag `override*`. No mira `isGlobal`, `size`, `priority`, `blendDistance` ni `weight`; eso lo aplica quien mezcla la pila. El motor lo llama al evaluar el volumen. Desde un script solo hace falta si quieres leer el perfil que saldría con los campos actuales.

**Devuelve** el perfil. Los efectos cuyo checkbox de override está apagado viajan en el struct, pero el blend los ignora.
