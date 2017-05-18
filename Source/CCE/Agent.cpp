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
  
  DistanceToGoal = CalculateDistanceToGoal();
 
  DistanceToTeammates = CalcualteDistanceToTeammates();
  
  DistanceToOpponents = CalcualteDistanceToOpponents();

  DistanceToWalls=CalcualteDistanceToWalls();
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
    DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Green, false, -1, 0, 2.0f);
    return (LinkEnd - LinkStart).Size();
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

std::vector<float> AAgent::CalcualteDistanceToTeammates() {

	std::vector<float> distances;
	TArray<AActor*> ClosestAgents = GetClosestAgents();
	for (AActor* Actor : ClosestAgents) {
		AAgent* Agent = Cast<AAgent>(Actor);
		if (Agent && Agent != this && Agent->GetTeam() == this->GetTeam())
		{
			FVector LinkStart = GetActorLocation();
			FVector LinkEnd = Agent->GetActorLocation();
			distances.push_back((LinkEnd - LinkStart).Size());
			//debug info
			FColor TeamColor = Agent->GetTeam() == 0 ? FColor::Red : FColor::Blue;
			DrawDebugLine(GetWorld(), LinkStart, LinkEnd, TeamColor, false, -1, 1, 1.0f);
		}
	}
	return distances;
}

std::vector<float> AAgent::CalcualteDistanceToOpponents() {

	std::vector<float> distances;
	TArray<AActor*> ClosestAgents = GetClosestAgents();
	for (AActor* Actor : ClosestAgents) {
		AAgent* Agent = Cast<AAgent>(Actor);
		if (Agent && Agent != this && Agent->GetTeam() != this->GetTeam())
		{
			FVector LinkStart = GetActorLocation();
			FVector LinkEnd = Agent->GetActorLocation();
			distances.push_back((LinkEnd - LinkStart).Size());
			//debug info
			FColor TeamColor = Agent->GetTeam() == 0 ? FColor::Red : FColor::Blue;
			DrawDebugLine(GetWorld(), LinkStart, LinkEnd, TeamColor, false, -1, 1, 1.0f);
		}
	}
	return distances;
}

TArray<AActor*> AAgent::GetClosestAgents() {

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAgent::StaticClass(), FoundActors);
	FoundActors.Sort([this](const AActor& FirstActor, const AActor& SecondActor) {
		FVector CurrActorLocation = this->GetActorLocation();
		FVector FirstActorLocation = FirstActor.GetActorLocation();
		FVector SecondActorLocation = SecondActor.GetActorLocation();
		float DistanceToFirstActor = (CurrActorLocation - FirstActorLocation).Size();
		float DistanceToSecondtActor = (CurrActorLocation - SecondActorLocation).Size();
		return DistanceToFirstActor > DistanceToSecondtActor; });
	return FoundActors;
}

std::vector<float> AAgent::CalcualteDistanceToWalls()
{
	std::vector<float> Result;
	float RayCastLenght = 10000.0f;
	FHitResult* HitResult = new FHitResult();
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();
	FVector StartTrace = this->GetActorLocation();

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (FVector::ForwardVector*RayCastLenght) + StartTrace, ECC_WorldStatic, *TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, HitResult->TraceEnd, FColor::Yellow, false, -1, 1, 2.0f);
		Result.push_back(HitResult->Distance);
	}
	else {
		Result.push_back(0.0f);
	}
	
	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (-FVector::ForwardVector*RayCastLenght) + StartTrace, ECC_WorldStatic, *TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, HitResult->TraceEnd, FColor::Yellow, false, -1, 1, 2.0f);
		Result.push_back(HitResult->Distance);
	}
	else {
		Result.push_back(0.0f);
	}

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (FVector::RightVector*RayCastLenght) + StartTrace, ECC_WorldStatic, *TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, HitResult->TraceEnd, FColor::Yellow, false, -1, 1, 2.0f);
		Result.push_back(HitResult->Distance);
	}
	else {
		Result.push_back(0.0f);
	}

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (-FVector::RightVector*RayCastLenght) + StartTrace, ECC_WorldStatic, *TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, HitResult->TraceEnd, FColor::Yellow, false, -1, 1, 2.0f);
		Result.push_back(HitResult->Distance);
	}
	else {
		Result.push_back(0.0f);
	}

	for (std::vector<float>::iterator it = Result.begin(); it != Result.end(); ++it) {
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString::SanitizeFloat(*it));
	}
	return Result;
}