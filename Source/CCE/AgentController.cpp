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
		Req.SetAcceptanceRadius(50);
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
	
	ControlAgent(LastCalculatedOutput);

	if (TicksFromLastCalculate++ > 60) {
		TicksFromLastCalculate = 0;
		LastCalculatedOutput = ActivateNeuralNetwork();
		//for (int i = 0; i < 4; i++) {
		//	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Black, FString::SanitizeFloat(LastCalculatedOutput[i]));
		//}
	}
	
}

double* AAgentController::ActivateNeuralNetwork()
{
	double* Input= new double[7];
	double* Output= new double[4];
	Input[0] = Agent->GetDistanceToBall() / 1000;
	Input[1] = Agent->GetDistanceToBall() / 1000;
	Input[2] = Agent->GetDistanceToBall() / 1000;
	Input[3] = Agent->GetDistanceToBall() / 1000;
	Input[4] = Agent->GetDistanceToBall() / 1000;
	Input[5] = Agent->GetDistanceToBall() / 1000;
	Input[6] = .5;

	NeuralNetwork->loadSensors(Input);

	int i = 0;
	std::vector<NEAT::NNode*>::iterator OutputIterator = NeuralNetwork->outputs.begin();
	for (OutputIterator = NeuralNetwork->outputs.begin(); OutputIterator != NeuralNetwork->outputs.end(); OutputIterator++, i++) {
		Output[i] = round((*OutputIterator)->activation);
	}

	return Output;
}

void AAgentController::ControlAgent(double *ControlValues)  {
	if (!ControlValues)
		return;
	int16 moveStep = 500;
	
	FVector MovementVector = FVector::ZeroVector;
	
	if(ControlValues[0] == 1)
		MovementVector+=FVector::ForwardVector * moveStep;
	
	if (ControlValues[1] == 1)
		MovementVector -= FVector::ForwardVector * moveStep;
	
	if (ControlValues[2] == 1)
		MovementVector += FVector::RightVector * moveStep;
	
	if (ControlValues[3] == 1)
		MovementVector -= FVector::RightVector * moveStep;

	Agent->AddMovementInput(MovementVector, moveStep);
}
