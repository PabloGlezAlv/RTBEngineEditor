# DLL boundary

`GameScripts.dll` and `RTBEngine.dll` do not safely share the same heap for STL containers. Passing a `std::string` or a `std::vector` by value crosses allocators, and the destructor frees memory from the wrong CRT.

## Do not do this

```cpp
std::string name = GetOwner()->GetName();
std::string msg = "Hola " + name;
RTB_INFO(msg);
```

## Do this

```cpp
const char* name = GetOwner()->GetNameCStr();
char msg[256];
snprintf(msg, sizeof(msg), "Hola %s", name);
RTB_INFO(msg);
```

`RTB_INFO`, `RTB_WARN`, and `RTB_ERROR` take `const char*`. The `Logger` lives in the engine.

## What can cross

- `int`, `float`, `bool`, raw pointers
- `const char*` from a literal or a stack buffer
- `Vector2`, `Vector3`, `Vector4`, `Quaternion`, `Matrix4`, `Color` (structs of floats)
- `GameObject*`, `Component*`, `Scene*`

## What cannot

- `std::string` by value, or by reference returned to the script so the script destroys it
- `std::vector` by value
- `unique_ptr` / `shared_ptr` through the public script API
- SDL types in headers the script includes. SDL stays inside the engine

`std::string` fields that are **members** of the component are valid as reflected properties: the engine reads bytes at the object's offset. It does not ask you to return the string. Do not `return nombre;` from an exported function.

## Names and logs

| You need | Use |
| --- | --- |
| Object name | `GetNameCStr()` |
| Log | `snprintf` + `RTB_INFO` |
| Child list | `GetChildCount()` / `GetChildAt(i)`, or the `GetChildren()` reference without copying it into a vector of yours that you then return |

`GetChildren()` returns a reference to the internal vector. Walk it in the same call. Do not keep it past the frame if the hierarchy can change, and do not copy it with the script allocator to hand it back to the engine.
