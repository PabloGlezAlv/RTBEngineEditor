# Logger

`RTBEngine::Core::Logger::GetInstance()` writes to the editor console and to the player log. In the editor, Info is filtered in gray, Warning in yellow, and Error in red.

`msg` is a `const char*`. Do not build a temporary `std::string` to pass in: its destructor would run on the `GameScripts` heap. See [DLL boundary](../manual/scripting/frontera-dll.md).

```cpp
char buf[256];
snprintf(buf, sizeof(buf), "Hit on %s", other->GetNameCStr());
RTB_INFO(buf);
```

## RTB_INFO

```cpp
#define RTB_INFO(msg)
```

Expands to `Logger::GetInstance().Info(msg)`. Writes an information line. A null `msg` is not a valid message: pass a literal or a buffer.

- `msg`: `const char*` text.

## RTB_WARN

```cpp
#define RTB_WARN(msg)
```

Expands to `Logger::GetInstance().Warning(msg)`. Same pointer rule as `RTB_INFO`. In the editor it shows in yellow.

- `msg`: `const char*` text.

## RTB_ERROR

```cpp
#define RTB_ERROR(msg)
```

Expands to `Logger::GetInstance().Error(msg)`. It does not abort the process: it only logs. In the editor it shows in red.

- `msg`: `const char*` text.
