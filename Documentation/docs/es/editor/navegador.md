# Navegador de contenido

El navegador parte de `Assets/`. Enseña una rejilla de iconos. Las carpetas van primero, luego los archivos, ambos en orden alfabético sin distinguir mayúsculas.

## Navegar

- Clic en una carpeta entra.
- La flecha atrás sube un nivel.
- Las migas de pan son clicables.
- Un clic selecciona y el Inspector reacciona.
- `F2` renombra. Enter confirma.
- `Supr` borra, con confirmación. Una carpeta se borra entera.

## Doble clic

| Tipo | Acción |
| --- | --- |
| Carpeta | Entrar |
| `.lua` | Pide cargar la escena. Solo en Edit, y solo si no es la ya abierta |
| `.prefab` | Abre el modo prefab. Si el prefab actual está dirty, pregunta |
| Resto | No abre una app externa |

Cargar otra escena **tira los cambios no guardados**. Guarda antes.

## Arrastrar

| Icono | Payload |
| --- | --- |
| Imagen | Textura |
| Modelo | Malla |
| Cubemap | Cubemap |
| FBX sobre un slot FBX | Ruta FBX |

Suéltalo en el campo compatible del Inspector.

## Clic derecho

**New**: carpeta, componente C++, clase C++ vacía, escena `.lua`, cubemap.

**Archivo**: renombrar, borrar, mostrar en el explorador.

El componente C++ pide el nombre y escribe el par `.h` / `.cpp` con `RTB_COMPONENT` y el registro vacío. Plantilla completa en [Componentes C++](../manual/scripting/componentes-cpp.md).
