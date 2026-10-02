# Console

The console shows the engine `Logger` on the same thread: every `RTB_INFO`, `RTB_WARN`, and `RTB_ERROR` from the engine or from a script.

```cpp
char msg[256];
snprintf(msg, sizeof(msg), "Vida %d", hp);
RTB_INFO(msg);
RTB_WARN("Collider ausente");
RTB_ERROR("Clip no encontrado");
```

Pass `const char*`. Do not pass a temporary `std::string`. See [DLL boundary](../manual/scripting/frontera-dll.md).

## Filters

Three buttons: Info, Warning, and Error. The search field is a case-sensitive substring. **Clear** empties the list. With auto-scroll, each new message jumps to the bottom if you were already there.

Colors: info gray, warning yellow, error red. Each line has a time and a level icon.

The console is not a terminal. It does not run commands.
