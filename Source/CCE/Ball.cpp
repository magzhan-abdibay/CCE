#include "CCE.h"
#include "Ball.h"
#include "Population.h"

ABall::ABall() {
  PrimaryActorTick.bCanEverTick = true;
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  RootComponent = Mesh;
}

void ABall::BeginPlay() { Super::BeginPlay(); }

void ABall::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ABall::DoStuff() {
	NEAT::Population *p = 0;
}
