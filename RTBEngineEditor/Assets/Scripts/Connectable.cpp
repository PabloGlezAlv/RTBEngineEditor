#include "Connectable.h"

using ThisClass = Connectable;
RTB_REGISTER_COMPONENT(Connectable)
RTB_END_REGISTER(Connectable)

Connectable::Connectable() {}
Connectable::~Connectable() {}

void Connectable::OnAwake() {}
void Connectable::OnStart() {}
void Connectable::OnUpdate(float deltaTime) {}
void Connectable::OnFixedUpdate(float fixedDeltaTime) {}
void Connectable::OnDestroy() {}
