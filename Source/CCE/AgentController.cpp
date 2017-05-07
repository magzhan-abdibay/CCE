#include "CCE.h"
#include "AgentController.h"

AAgentController::AAgentController() {}

void AAgentController::Possess(APawn *Value) {
  Super::Possess(Value);
  Agent = Cast<AAgent>(Value);
}

void AAgentController::MoveTo(FVector Dest) {
	UNavigationPath* Path = UNavigationSystem::FindPathToLocationSynchronously(this, Agent->GetActorLocation(), Dest, Agent);

	if (Path && Path->IsValid())
	{
		FAIMoveRequest Req;
		//Req.SetAcceptanceRadius(50);
		Req.SetUsePathfinding(true);

		AAIController* AiController = Cast<AAIController>(Agent->GetController());
		if (AiController)
		{
			AiController->RequestMove(Req, Path->GetPath());
		}
	}
}

void AAgentController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	if (TicksFromLastCalculate++ > 10) {
		TicksFromLastCalculate = 0;

		EPathFollowingStatus::Type status = GetMoveStatus();
		if (status == EPathFollowingStatus::Moving) {
			//GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Emerald, "Moving");
		}
		else if (status == EPathFollowingStatus::Idle) {
			double value = DoStuff();
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString::SanitizeFloat(value * 1000));
			FVector Destination = FVector(value * 1000, value * 10, 230);
			MoveTo(Destination);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Black, "Else");
		}
	}
}

double AAgentController::DoStuff()
{
	double input[7];
	input[0] = Agent->GetDistanceToBall() / 10000;
	input[1] = Agent->GetDistanceToGoal() / 10000;
	input[2] = Agent->GetDistanceToTeammate() / 10000;
	input[3] = Agent->GetDistanceToBall() / 10000;
	input[4] = Agent->GetDistanceToBall() / 10000;
	input[5] = Agent->GetDistanceToBall() / 10000;
	input[6] = .5;

	NeuralNetwork->loadSensors(input);

	//Activate the net
	//If it loops, exit returning only fitness of 1 step
	if (!(NeuralNetwork->activate())) return 1;

	return  (*(NeuralNetwork->outputs.begin()))->activation;
}

FVector AAgentController::GenerateTargetVector(FVector &value)  {
	int32 orientation = FMath::RandRange(1, 4);
	int32 moveStep = 500;
	FVector result;
	switch (orientation) {
	case 1:

		result = FVector::ForwardVector * moveStep;
		break;
	case 2:
		result = -FVector::ForwardVector * moveStep;
		break;
	case 3:
		result = FVector::RightVector * moveStep;
		break;
	case 4:
		result = -FVector::RightVector * moveStep;
		break;
	default:
		result = FVector::ZeroVector;
	}
	result += value;
	return result;
}
