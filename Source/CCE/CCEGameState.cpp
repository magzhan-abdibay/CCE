#include "CCE.h"
#include "CCEGameState.h"

ACCEGameState::ACCEGameState() { PrimaryActorTick.bCanEverTick = true; }

void ACCEGameState::Tick(float DeltaTime) {Super::Tick(DeltaTime);}
