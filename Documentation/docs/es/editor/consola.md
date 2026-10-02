# Consola

La consola muestra el `Logger` del motor en el mismo hilo: cada `RTB_INFO`, `RTB_WARN` y `RTB_ERROR` del motor o de un script.

```cpp
char msg[256];
snprintf(msg, sizeof(msg), "Vida %d", hp);
RTB_INFO(msg);
RTB_WARN("Collider ausente");
RTB_ERROR("Clip no encontrado");
```

Pasa `const char*`. No pases un `std::string` temporal. Ver [Frontera DLL](../manual/scripting/frontera-dll.md).

## Filtros

Tres botones: Info, Warning y Error. El campo de búsqueda es una subcadena, sensible a mayúsculas. **Clear** vacía la lista. Con auto-scroll, cada mensaje nuevo baja al final si ya estabas al final.

Colores: info gris, warning amarillo, error rojo. Cada línea lleva hora y un icono de nivel.

La consola no es un terminal. No ejecuta comandos.
