# Documentación de RTBEngine

Sitio del manual, del editor y de la Scripting API. Español en la raíz e inglés en `/en/`.

La fuente está en `docs/es` y `docs/en`. El tema es [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/), con la misma estructura de pestañas que un manual de motor: Manual, Editor, Online y Scripting API.

## Vista local

Desde `RTBEngineEditor/Documentation`:

```powershell
py -3 -m pip install -r requirements.txt
py -3 -m mkdocs serve
```

Abre `http://127.0.0.1:8000`.

## Publicar en GitHub Pages

El workflow `.github/workflows/docs.yml` construye el sitio y lo publica al hacer push a `main` (si cambió `Documentation/` o el propio workflow).

1. En el repositorio `RTBEngineEditor`: **Settings → Pages → Build and deployment → Source: GitHub Actions**.
2. Push a `main`.
3. El sitio queda en `https://pabloglezalv.github.io/RTBEngineEditor/`.

La rama por defecto del editor es `main`.
