#include "CCE.h"
#include "Agent.h"
#include "Ball.h"
#include "Goal.h"

AAgent::AAgent() {
  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  GetCharacterMovement()->bOrientRotationToMovement = true; 
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

void AAgent::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	FLinearColor BodyColor = Team == 0 ? FLinearColor::Red : FLinearColor::Blue;
	DynamicMaterial->SetVectorParameterValue("BodyColor", BodyColor);
	GetMesh()->SetMaterial(0, DynamicMaterial);
}

float AAgent::CalculateDistanceToBall() {
  for (TActorIterator<ABall> BallItr(GetWorld()); BallItr; ++BallItr) {
    FVector LinkStart = GetActorLocation();
    FVector LinkEnd = BallItr->GetActorLocation();
    DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Green, false, -1, 0, 1.0f);
    return (LinkEnd - LinkStart).Size();
  }
  return 0.0f;
}

float AAgent::CalcualteDistanceToTeammate() {
  for (TActorIterator<AAgent> AgentItr(GetWorld()); AgentItr; ++AgentItr) {
    if (*AgentItr != this && AgentItr->GetTeam() == this->Team) {
      FVector LinkStart = GetActorLocation();
      FVector LinkEnd = AgentItr->GetActorLocation();
      DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Red, false, -1, 1, 2.0f);
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
      DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Cyan, false, -1, 1, 1.0f);
      return (LinkEnd - LinkStart).Size();
    }
  }
  return 0.0f;
}
