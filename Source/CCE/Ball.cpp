#include "CCE.h"
#include "Ball.h"

ABall::ABall() {
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  RootComponent = Mesh;
}

void ABall::BeginPlay() { 
	Super::BeginPlay();	
}