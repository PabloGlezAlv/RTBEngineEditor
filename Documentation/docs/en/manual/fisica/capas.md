# Layers

Every `GameObject` has a collision-layer index.

```cpp
int layer = go->GetCollisionLayer();
go->SetCollisionLayer(2);
go->SetCollisionLayerByName("Player");
```

The matrix says which layer pairs generate contacts. The player can avoid colliding with their own camera trigger, and projectiles can ignore their owner.

## In the editor

**Window → Physics Layers** opens the panel (closed by default). Names and the matrix of the active project are edited there.

- **Save** writes the project settings.
- **Reset to engine default** restores the factory matrix.

The panel does not simulate anything. It only changes which pairs Bullet will query the next time the scene builds the world.

## Practice

1. Put the player on `Player` and their team sensors on another layer.
2. Uncheck the pair that must not generate `OnTriggerEnter`.
3. Save. Enter Play and check the callback with an `RTB_INFO`.

If an object "does not collide with anything", check in this order: collider present, body present, `bodyType` correct, layer not excluded against the world, and that you are not in Edit.
