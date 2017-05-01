#include "CCE.h"
#include "Ball.h"
#include "Population.h"
#include "experiments.h"

ABall::ABall() {
  PrimaryActorTick.bCanEverTick = true;
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  RootComponent = Mesh;
}

void ABall::BeginPlay() { 
	Super::BeginPlay(); 
	
	DoStuff(); 
}

void ABall::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ABall::DoStuff() {
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue,"start");
	char* path = "E:\\UnrealProjects\\CCE\\NEAT\\NEATConsoleApplication\\pole2_markov.ne";
	NEAT::loadNeatParams(path, true);

	NEAT::Population *p = 0;
	p = pole2TestRealTime();
	if (p) {
		delete p;
	}
}
