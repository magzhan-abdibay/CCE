#include "CCE.h"
#include "Goal.h"
#include "Ball.h"
#include "CCEGameState.h"
#include "DrawDebugHelpers.h"

AGoal::AGoal() {
  PrimaryActorTick.bCanEverTick = true;
  Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
  Box->bGenerateOverlapEvents = true;
  Box->OnComponentBeginOverlap.AddDynamic(this, &AGoal::TriggerEnter);
  Box->OnComponentEndOverlap.AddDynamic(this, &AGoal::TriggerExit);
  RootComponent = Box;
}

void AGoal::BeginPlay() { Super::BeginPlay(); }

void AGoal::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AGoal::TriggerEnter(UPrimitiveComponent *HitComp, AActor *OtherActor,
                         UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
                         bool bFromSweep, const FHitResult &SweepResult) {
  if (OtherActor->IsA(ABall::StaticClass())) {
    FVector ActorLocation = OtherActor->GetActorLocation();
    OtherActor->Destroy();

    ActorLocation.X = 0.0f;
    ActorLocation.Y = 9.0f;
    FRotator ActorRotation(0.0f, 0.0f, 0.0f);
    FActorSpawnParameters SpawnInfo;
    GetWorld()->SpawnActor<ABall>(Ball, ActorLocation, ActorRotation,
                                  SpawnInfo);

    OtherActor->SetActorLocation(ActorLocation);
    ACCEGameState *const GameState =
        GetWorld() != NULL ? GetWorld()->GetGameState<ACCEGameState>() : NULL;

    FVector velocity = FVector(0.0f, 0.0f, 0.0f);
    OtherActor->GetRootComponent()->ComponentVelocity = velocity;
    if (GameState != NULL) {
      if (Team == 0) {
        GameState->SetScoreTeam1(GameState->GetScoreTeam1() + 1);
      } else if (Team == 1) {
        GameState->SetScoreTeam0(GameState->GetScoreTeam0() + 1);
      }
      GEngine->AddOnScreenDebugMessage(
          -1, 7.f, FColor::Blue, FString::FromInt(GameState->GetScoreTeam0()));
      GEngine->AddOnScreenDebugMessage(
          -1, 7.f, FColor::Blue, FString::FromInt(GameState->GetScoreTeam1()));
    }
  }
}

void AGoal::TriggerExit(class UPrimitiveComponent *HitComp,
                        class AActor *OtherActor,
                        class UPrimitiveComponent *OtherComp,
                        int32 OtherBodyIndex) {
  if (OtherActor->IsA(ABall::StaticClass())) {
    //TODO: do something or delete
  }
}
