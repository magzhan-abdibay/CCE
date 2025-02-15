#include "CCE.h"
#include "Agent.h"
#include "Ball.h"
#include "Goal.h"

AAgent::AAgent()
{
}

void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DistanceToBall = CalculateDistanceToBall();

	DistanceToGoal = CalculateDistanceToGoal();

	DistanceToTeammates = CalcualteDistanceToTeammates();

	DistanceToOpponents = CalcualteDistanceToOpponents();

	DistanceToWalls = CalcualteDistanceToWalls();
}

void AAgent::DifferentiateTeamsByColor()
{
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	FLinearColor BodyColor = Team == 0 ? FLinearColor::Blue : FLinearColor::Red;
	DynamicMaterial->SetVectorParameterValue("BodyColor", BodyColor);
	GetMesh()->SetMaterial(0, DynamicMaterial);
}

float AAgent::CalculateDistanceToBall() const
{
	for (TActorIterator<ABall> BallItr(GetWorld()); BallItr; ++BallItr)
	{
		FVector LinkStart = GetActorLocation();
		FVector LinkEnd = BallItr->GetActorLocation();
#if !UE_BUILD_SHIPPING
		DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Green, false, -1, 0);
#endif
		return (LinkEnd - LinkStart).Size();
	}
	return 0.0f;
}

float AAgent::CalculateDistanceToGoal() const
{
	for (TActorIterator<AGoal> GoalItr(GetWorld()); GoalItr; ++GoalItr)
	{
		if (GoalItr->GetTeam() != this->Team)
		{
			FVector LinkStart = GetActorLocation();
			FVector LinkEnd = GoalItr->GetActorLocation();
#if !UE_BUILD_SHIPPING
			DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Cyan, false, -1, 1);
#endif
			return (LinkEnd - LinkStart).Size();
		}
	}
	return 0.0f;
}

std::vector<float> AAgent::CalcualteDistanceToTeammates()
{
	std::vector<float> distances;
	TArray<AActor*> ClosestAgents = GetClosestAgents();
	for (AActor* Actor : ClosestAgents)
	{
		AAgent* Agent = Cast<AAgent>(Actor);
		if (Agent && Agent != this && Agent->GetTeam() == this->GetTeam())
		{
			FVector LinkStart = GetActorLocation();
			FVector LinkEnd = Agent->GetActorLocation();
			distances.push_back((LinkEnd - LinkStart).Size());
#if !UE_BUILD_SHIPPING
			FColor TeamColor = Agent->GetTeam() == 0 ? FColor::Blue : FColor::Red;
			DrawDebugLine(GetWorld(), LinkStart, LinkEnd, TeamColor, false, -1, 1);
#endif
		}
	}
	return distances;
}

std::vector<float> AAgent::CalcualteDistanceToOpponents()
{
	std::vector<float> distances;
	TArray<AActor*> ClosestAgents = GetClosestAgents();
	for (AActor* Actor : ClosestAgents)
	{
		AAgent* Agent = Cast<AAgent>(Actor);
		if (Agent && Agent != this && Agent->GetTeam() != this->GetTeam())
		{
			FVector LinkStart = GetActorLocation();
			FVector LinkEnd = Agent->GetActorLocation();
			distances.push_back((LinkEnd - LinkStart).Size());
#if !UE_BUILD_SHIPPING
			FColor OpponentTeamColor = Agent->GetTeam() == 0 ? FColor::Red : FColor::Blue;
			DrawDebugLine(GetWorld(), LinkStart, LinkEnd, OpponentTeamColor, false, -1, 1);
#endif
		}
	}
	return distances;
}

TArray<AActor*> AAgent::GetClosestAgents()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAgent::StaticClass(), FoundActors);
	FoundActors.Sort([this](const AActor& FirstActor, const AActor& SecondActor)
	{
		FVector CurrActorLocation = this->GetActorLocation();
		FVector FirstActorLocation = FirstActor.GetActorLocation();
		FVector SecondActorLocation = SecondActor.GetActorLocation();
		float DistanceToFirstActor = (CurrActorLocation - FirstActorLocation).Size();
		float DistanceToSecondtActor = (CurrActorLocation - SecondActorLocation).Size();
		return DistanceToFirstActor > DistanceToSecondtActor;
	});
	return FoundActors;
}

std::vector<float> AAgent::CalcualteDistanceToWalls() const
{
	std::vector<float> Result;
	float RayCastLenght = 10000.0f;
	FHitResult* HitResult = new FHitResult();
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();
	FVector StartTrace = this->GetActorLocation();

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (FVector::ForwardVector * RayCastLenght) + StartTrace, ECC_Visibility, *TraceParams))
	{
#if !UE_BUILD_SHIPPING
		DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Purple, false, -1, 1);
#endif
		Result.push_back(HitResult->Distance);
	}
	else
	{
		Result.push_back(0.0f);
	}

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (-FVector::ForwardVector * RayCastLenght) + StartTrace, ECC_Visibility, *TraceParams))
	{
#if !UE_BUILD_SHIPPING
		DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Purple, false, -1, 1);
#endif
		Result.push_back(HitResult->Distance);
	}
	else
	{
		Result.push_back(0.0f);
	}

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (FVector::RightVector * RayCastLenght) + StartTrace, ECC_Visibility, *TraceParams))
	{
#if !UE_BUILD_SHIPPING
		DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Purple, false, -1, 1);
#endif
		Result.push_back(HitResult->Distance);
	}
	else
	{
		Result.push_back(0.0f);
	}

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (-FVector::RightVector * RayCastLenght) + StartTrace, ECC_Visibility, *TraceParams))
	{
#if !UE_BUILD_SHIPPING
		DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Purple, false, -1, 1);
#endif
		Result.push_back(HitResult->Distance);
	}
	else
	{
		Result.push_back(0.0f);
	}

	return Result;
}
