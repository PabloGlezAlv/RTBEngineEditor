# Capas

Cada `GameObject` tiene un índice de capa de colisión.

```cpp
int layer = go->GetCollisionLayer();
go->SetCollisionLayer(2);
go->SetCollisionLayerByName("Player");
```

La matriz dice qué pares de capas generan contactos. El jugador puede no chocar con el trigger de su propia cámara, y los proyectiles pueden ignorar al dueño.

## En el editor

**Window → Physics Layers** abre el panel (cerrado por defecto). Ahí se editan los nombres y la matriz del proyecto activo.

- **Save** escribe los ajustes del proyecto.
- **Reset to engine default** restaura la matriz de fábrica.

El panel no simula nada. Solo cambia qué pares consultará Bullet la próxima vez que la escena construya el mundo.

## Práctica

1. Pon al jugador en `Player` y a sus sensores de equipo en otra capa.
2. Desmarca el par que no debe generar `OnTriggerEnter`.
3. Guarda. Entra en Play y comprueba el callback con un `RTB_INFO`.

Si un objeto “no colisiona con nada”, revisa en este orden: collider presente, cuerpo presente, `bodyType` correcto, capa no excluida contra el mundo, y que no estés en Edit.
