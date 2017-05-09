#include "CCE.h"
#include "AgentController.h"

AAgentController::AAgentController() {}

void AAgentController::Possess(APawn *Value) {
  Super::Possess(Value);
  Agent = Cast<AAgent>(Value);
}

void AAgentController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	ControlAgent(LastCalculatedOutput);

	if (TicksFromLastCalculate++ > 4) {
		TicksFromLastCalculate = 0;
		LastCalculatedOutput = ActivateNeuralNetwork();
		NeatOrganism->fitness = EvaluateFitness();
	}
	
}

double* AAgentController::ActivateNeuralNetwork()
{
	//GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("Activating")));
	double* Input= new double[7];
	double* Output= new double[4];
	Input[0] = Agent->GetDistanceToBall() ;
	Input[1] = Agent->GetDistanceToGoal() ;
	Input[2] = Agent->GetDistanceToTeammate();
	Input[3] = Agent->GetDistanceToBall();
	Input[4] = Agent->GetDistanceToBall();
	Input[5] = Agent->GetDistanceToBall();
	Input[6] = .5;

	//for (int i = 0; i < 7; i++) {
		//GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString::SanitizeFloat(Input[i]));
	//}

	NeatOrganism->net->loadSensors(Input);

	NeatOrganism->net->activate();

	int i = 0;
	std::vector<NEAT::NNode*>::iterator OutputIterator = NeatOrganism->net->outputs.begin();
	for (OutputIterator = NeatOrganism->net->outputs.begin(); OutputIterator != NeatOrganism->net->outputs.end(); OutputIterator++, i++) {
		Output[i] = (*OutputIterator)->activation;
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString::SanitizeFloat((*OutputIterator)->activation));
	}

	return Output;
}

double AAgentController::EvaluateFitness() {
	double Result = 0.0f;
	if (LastCalculatedOutput) {
		for (int i = 0; i < 4; i++) {
			Result += LastCalculatedOutput[i];
		}
	}
	return Result;
}

void AAgentController::ControlAgent(double *ControlValues)  {
	if (!ControlValues)
		return;
	int16 moveStep = 500;
	
	FVector MovementVector = FVector::ZeroVector;
	
	if(ControlValues[0] == 1)
		MovementVector+=FVector::ForwardVector;
	
	if (ControlValues[1] == 1)
		MovementVector -= FVector::ForwardVector;
	
	if (ControlValues[2] == 1)
		MovementVector += FVector::RightVector;
	
	if (ControlValues[3] == 1)
		MovementVector -= FVector::RightVector;

	Agent->AddMovementInput(MovementVector, moveStep);
}
