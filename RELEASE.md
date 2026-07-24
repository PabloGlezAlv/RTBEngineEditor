# Release — RTBEngineEditor

Publishes a **ready-to-run editor** bundle (engine, DLLs, `Assets/`, `Default/`, `RTBPlayer.exe`, and `GameScripts.dll`).

## When it runs

When you push a semver tag (for example `0.10.0`) to this repository.

## Create a release

1. Ensure **RTBEngine** has the same tag on GitHub.
2. In this repo:

```bash
git checkout main
git pull
git tag 0.10.0
git push origin 0.10.0
```

The workflow [`.github/workflows/release.yml`](.github/workflows/release.yml) produces:

- `RTBEngineEditor-0.10.0-win-x64.zip`

Extract the archive and run `RTBEngineEditor.exe`.

## Private repositories

If **RTBEngine** is private, add a `GH_PAT` secret (Personal Access Token with read access to the engine repo). The workflow uses `secrets.GH_PAT || github.token` to clone the engine at the same tag.

## Zip contents

- `RTBEngineEditor.exe` + runtime DLLs
- `Assets/`, `Default/`
- `RTBEngine_SDK/` (embedded SDK)
- `RTBPlayer.exe` (game export from File → Build)
- `GameScripts.dll`

## Versioning

Must match the engine SDK version (`EditorVersion.h` / changelog).
