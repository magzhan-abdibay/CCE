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

	AgeInTicks++;

	DistanceToBall = CalculateDistanceToBall();

	AngleToBall = CalculateAngleToBall();

	DistanceToGoal = CalculateDistanceToGoal();

	DistanceToForwardObstacle = CalculateDistanceToForwardObstacle();

	DistanceToTeammates = CalcualteDistanceToTeammates();

	DistanceToOpponents = CalcualteDistanceToOpponents();

	//	DistanceToWalls = CalcualteDistanceToWalls();

	DistanceCircular = CalcualteDistanceCircular();

	TotalDistanceTravelled = CalculateTotalDistanceTravelled();
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
		if (DrawDebugLines)
			DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Green, false, -1, 0);
		return (LinkEnd - LinkStart).Size();
	}
	return 0.0f;
}

float AAgent::CalculateDistanceToForwardObstacle() const
{
	float RayCastLenght = 300.0f;
	float Result = RayCastLenght;
	FHitResult* HitResult = new FHitResult();
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();
	FVector StartTrace = this->GetActorLocation();
	FVector EndTrace = this->GetActorForwardVector() * RayCastLenght + StartTrace;

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Visibility, *TraceParams))
	{
		if (DrawDebugLines)
			DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Purple, false, -1, 1, 15.0f);
		Result = HitResult->Distance;
	}
	return Result;
}


float AAgent::CalculateAngleToBall() const
{
	for (TActorIterator<ABall> BallItr(GetWorld()); BallItr; ++BallItr)
	{
		FVector LinkStart = GetActorLocation();
		FVector LinkEnd = BallItr->GetActorLocation();
		if (DrawDebugLines)
			DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Green, false, -1, 0);

		FVector VectorToBall = LinkEnd - LinkStart;
		VectorToBall.Normalize();
		float result = FMath::RadiansToDegrees(acosf(FVector::DotProduct(VectorToBall, GetActorForwardVector())));
		//		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString::SanitizeFloat(result));
		return result;
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
			if (DrawDebugLines)
				DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Cyan, false, -1, 1);
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
			FColor TeamColor = Agent->GetTeam() == 0 ? FColor::Blue : FColor::Red;
			if (DrawDebugLines)
				DrawDebugLine(GetWorld(), LinkStart, LinkEnd, TeamColor, false, -1, 1);
		}
	}
	return distances;
}

void AAgent::Kick()
{
	float KickDistance = 200.0f;
	float KickPower = 800.0f;
	float BallHalfHeight = 20.0f;

	FHitResult* HitResult = new FHitResult();
	FVector StartTrace = GetCapsuleComponent()->GetComponentLocation() - FVector::UpVector * (GetCapsuleComponent()->GetScaledCapsuleRadius() - BallHalfHeight);
	FVector ForwardVector = RootComponent->GetForwardVector();
	FVector EndTrace = ((ForwardVector * KickDistance) + StartTrace);
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, EndTrace, ECC_Visibility, *TraceParams))
	{
		if (HitResult->GetActor()->GetClass()->IsChildOf(ABall::StaticClass()))
		{
			HitResult->GetComponent()->AddImpulse((ForwardVector * KickPower), NAME_None, true);
			DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Red, false);
			NumberOfKicks++;

			ABall* ball = static_cast<ABall*>(HitResult->GetActor());
			if (ball != nullptr)
			{
				ball->SetLastKickedAgent(this);
			}
		}
	}
	else
	{
		NumberOfFalseKicks++;
	}
}

void AAgent::BeginPlay()
{
	Super::BeginPlay();
	LastLocation = GetActorLocation();
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
			FColor OpponentTeamColor = Agent->GetTeam() == 0 ? FColor::Red : FColor::Blue;
			if (DrawDebugLines)
				DrawDebugLine(GetWorld(), LinkStart, LinkEnd, OpponentTeamColor, false, -1, 1);
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
		if (DrawDebugLines)
			DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Purple, false, -1, 1);
		Result.push_back(HitResult->Distance);
	}
	else
	{
		Result.push_back(0.0f);
	}

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (-FVector::ForwardVector * RayCastLenght) + StartTrace, ECC_Visibility, *TraceParams))
	{
		if (DrawDebugLines)
			DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Purple, false, -1, 1);
		Result.push_back(HitResult->Distance);
	}
	else
	{
		Result.push_back(0.0f);
	}

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (FVector::RightVector * RayCastLenght) + StartTrace, ECC_Visibility, *TraceParams))
	{
		if (DrawDebugLines)
			DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Purple, false, -1, 1);
		Result.push_back(HitResult->Distance);
	}
	else
	{
		Result.push_back(0.0f);
	}

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, (-FVector::RightVector * RayCastLenght) + StartTrace, ECC_Visibility, *TraceParams))
	{
		if (DrawDebugLines)
			DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Purple, false, -1, 1);
		Result.push_back(HitResult->Distance);
	}
	else
	{
		Result.push_back(0.0f);
	}

	return Result;
}

std::vector<float> AAgent::CalcualteDistanceCircular() const
{
	std::vector<float> Result;
	float RayCastLenght = 5000.0f;
	float CapsuleRadius = 45.0f;
	FHitResult* HitResult = new FHitResult();
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();
	FVector StartTrace = this->GetActorLocation();

	std::vector<FVector> EndTraceDirections;

	for (int i = -4; i <= 4; i++)
	{
			FVector EndTraceDirection = GetActorForwardVector().RotateAngleAxis(i * 40, GetActorUpVector());
			EndTraceDirections.push_back(EndTraceDirection);
	}


	for (int i = 0; i <= 8; i++) 
	{
		FVector EndTraceDirection =EndTraceDirections.at(i);
		if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace+ CapsuleRadius*EndTraceDirection, EndTraceDirection* RayCastLenght + StartTrace, ECC_OverlapAll_Deprecated, *TraceParams))
		{
			//		if (DrawDebugLines)
			DrawDebugLine(GetWorld(), StartTrace, HitResult->ImpactPoint, FColor::Green, false, -1, 1, 1.5f);
			Result.push_back(HitResult->Distance);
		}
		else
		{
			Result.push_back(RayCastLenght);
		}
	}

	return Result;
}

long AAgent::CalculateTotalDistanceTravelled()
{
	TotalDistanceTravelled = TotalDistanceTravelled + (GetActorLocation() - LastLocation).Size();
	LastLocation = GetActorLocation();
	return TotalDistanceTravelled;
}
