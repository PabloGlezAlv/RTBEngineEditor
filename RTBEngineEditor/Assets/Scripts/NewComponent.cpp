#include "NewComponent.h"

using ThisClass = NewComponent;
RTB_REGISTER_COMPONENT(NewComponent)
RTB_END_REGISTER(NewComponent)

NewComponent::NewComponent() {}
NewComponent::~NewComponent() {}

void NewComponent::OnAwake() {}

void NewComponent::OnStart() {}

void NewComponent::OnUpdate(float deltaTime) {}

void NewComponent::OnFixedUpdate(float fixedDeltaTime) {}

void NewComponent::OnDestroy() {}
