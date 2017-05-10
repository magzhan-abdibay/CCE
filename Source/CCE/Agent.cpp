#include "CCE.h"
#include "Agent.h"
#include "Ball.h"
#include "Goal.h"

AAgent::AAgent() {
  // Set size for collision capsule
  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

  // Don't rotate when the controller rotates. Let that just affect the camera.
  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  // Configure character movement
  GetCharacterMovement()->bOrientRotationToMovement = true; 
  // Character moves in the direction of input...
  // ...at this rotation rate
  GetCharacterMovement()->RotationRate =FRotator(0.0f, 540.0f, 0.0f); 
  GetCharacterMovement()->JumpZVelocity = 600.f;
  GetCharacterMovement()->AirControl = 0.2f; 
}

void AAgent::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  DistanceToBall = CalculateDistanceToBall();
  //GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red,
  //                                 FString::SanitizeFloat(DistanceToBall));

  DistanceToTeammate = CalcualteDistanceToTeammate();
  //GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan,
  //                                 FString::SanitizeFloat(DistanceToTeammate));

  DistanceToGoal = CalculateDistanceToGoal();
  //GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan,
  //                                 FString::SanitizeFloat(DistanceToGoal));
}

float AAgent::CalculateDistanceToBall() {
  for (TActorIterator<ABall> BallItr(GetWorld()); BallItr; ++BallItr) {
    FVector LinkStart = GetActorLocation();
    FVector LinkEnd = BallItr->GetActorLocation();
    DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Green, false, -1, 0,
                  2.0f);
    return (LinkEnd - LinkStart).Size();
  }
  return 0.0f;
}

float AAgent::CalcualteDistanceToTeammate() {
  for (TActorIterator<AAgent> AgentItr(GetWorld()); AgentItr; ++AgentItr) {
    if (*AgentItr != this && AgentItr->GetTeam() == this->Team) {
      FVector LinkStart = GetActorLocation();
      FVector LinkEnd = AgentItr->GetActorLocation();
      DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Red, false, -1, 1,
                    2.0f);
      return (LinkEnd - LinkStart).Size();
    }
  }
  return 0.0f;
}

float AAgent::CalculateDistanceToGoal() {
  for (TActorIterator<AGoal> GoalItr(GetWorld()); GoalItr; ++GoalItr) {
    if (GoalItr->GetTeam() != this->Team) {
      FVector LinkStart = GetActorLocation();
      FVector LinkEnd = GoalItr->GetActorLocation();
      DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Cyan, false, -1, 1,
                    2.0f);
      // GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green,
      // FString::SanitizeFloat((LinkEnd - LinkStart).Size()));
      return (LinkEnd - LinkStart).Size();
    }
  }
  return 0.0f;
}
