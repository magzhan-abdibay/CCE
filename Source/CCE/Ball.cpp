#include "CCE.h"
#include "Ball.h"

ABall::ABall() {
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  RootComponent = Mesh;
}

void ABall::BeginPlay() { 
	Super::BeginPlay();
	//Mesh->SetPhysicsLinearVelocity(FVector(10050.f, 5000, 0));
	Mesh->AddImpulse(FVector(1000, 0, 0),NAME_None,true);
}