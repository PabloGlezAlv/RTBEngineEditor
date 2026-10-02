# Frontera DLL

`GameScripts.dll` y `RTBEngine.dll` no comparten el mismo heap de forma segura para contenedores de la STL. Pasar un `std::string` o un `std::vector` por valor cruza el asignador y el destructor libera memoria del CRT equivocado.

## No hagas esto

```cpp
std::string name = GetOwner()->GetName();
std::string msg = "Hola " + name;
RTB_INFO(msg);
```

## Haz esto

```cpp
const char* name = GetOwner()->GetNameCStr();
char msg[256];
snprintf(msg, sizeof(msg), "Hola %s", name);
RTB_INFO(msg);
```

`RTB_INFO`, `RTB_WARN` y `RTB_ERROR` aceptan `const char*`. El `Logger` vive en el motor.

## Qué sí puede cruzar

- `int`, `float`, `bool`, punteros crudos
- `const char*` de un literal o de un buffer de pila
- `Vector2`, `Vector3`, `Vector4`, `Quaternion`, `Matrix4`, `Color` (structs de floats)
- `GameObject*`, `Component*`, `Scene*`

## Qué no

- `std::string` por valor o por referencia devuelta al script para que la destruya el script
- `std::vector` por valor
- `unique_ptr` / `shared_ptr` a través de la API pública del script
- Tipos SDL en headers que incluya el script. SDL se queda dentro del motor

Los campos `std::string` **miembros** del componente son válidos como propiedades reflejadas: el motor lee bytes en el offset del objeto, no te pide que devuelvas el string. No hagas `return nombre;` desde una función exportada.

## Nombres y logs

| Necesitas | Usa |
| --- | --- |
| Nombre del objeto | `GetNameCStr()` |
| Log | `snprintf` + `RTB_INFO` |
| Lista de hijos | `GetChildCount()` / `GetChildAt(i)` o la referencia `GetChildren()` sin copiarla a un vector tuyo que luego devuelvas |

`GetChildren()` devuelve una referencia al vector interno. Recórrela en el mismo call. No la guardes más allá del frame si la jerarquía puede cambiar, y no la copies con el asignador del script para devolverla al motor.
