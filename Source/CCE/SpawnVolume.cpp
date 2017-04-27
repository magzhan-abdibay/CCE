#include "CCE.h"
#include "SpawnVolume.h"
#include "Agent.h"
#include "Kismet/KismetMathLibrary.h"

ASpawnVolume::ASpawnVolume() {
  PrimaryActorTick.bCanEverTick = false;

  // Create the Box Component to represent the spawn volume
  WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
  RootComponent = WhereToSpawn;
}

void ASpawnVolume::BeginPlay() {
  Super::BeginPlay();
  SpawnAgent();
}

void ASpawnVolume::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

FVector ASpawnVolume::GetRandomPointInVolume() {
  FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
  FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;
  return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

void ASpawnVolume::SpawnAgent() {
  // If we have set something to spawn:
  if (WhatToSpawn != NULL) {
    // Check for a valid World:
    UWorld *const World = GetWorld();
    if (World) {
      // Set the spawn parameters
      FActorSpawnParameters SpawnParams;
      SpawnParams.Owner = this;
      SpawnParams.Instigator = Instigator;

      // Get a random location to spawn at
      FVector SpawnLocation = GetRandomPointInVolume();

      // Get a random rotation for the spawned item
      FRotator SpawnRotation(0, FMath::FRand() * 360.0f, 0);

      // spawn
      AAgent *const SpawnedActor = World->SpawnActor<AAgent>(
          WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
    }
  }
}
