# Logger

`RTBEngine::Core::Logger::GetInstance()` escribe en la consola del editor y en el log del player. En el editor, Info se filtra en gris, Warning en amarillo y Error en rojo.

`msg` es `const char*`. No construyas un `std::string` temporal para pasarlo: el destructor correría en el heap de `GameScripts`. Ver [Frontera DLL](../manual/scripting/frontera-dll.md).

```cpp
char buf[256];
snprintf(buf, sizeof(buf), "Impacto en %s", other->GetNameCStr());
RTB_INFO(buf);
```

## RTB_INFO

```cpp
#define RTB_INFO(msg)
```

Expande a `Logger::GetInstance().Info(msg)`. Escribe una línea de información. Un `msg` null no es un mensaje válido: pasa un literal o un buffer.

- `msg`: texto `const char*`.

## RTB_WARN

```cpp
#define RTB_WARN(msg)
```

Expande a `Logger::GetInstance().Warning(msg)`. Misma regla de puntero que `RTB_INFO`. En el editor sale en amarillo.

- `msg`: texto `const char*`.

## RTB_ERROR

```cpp
#define RTB_ERROR(msg)
```

Expande a `Logger::GetInstance().Error(msg)`. No aborta el proceso: solo registra. En el editor sale en rojo.

- `msg`: texto `const char*`.
