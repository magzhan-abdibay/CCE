#include "CCE.h"
#include "AgentController.h"

AAgentController::AAgentController()
{
}

void AAgentController::Possess(APawn* Value)
{
	Super::Possess(Value);
	Agent = Cast<AAgent>(Value);
}

void AAgentController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ControlAgent(LastCalculatedOutput);

	if (TicksFromLastCalculate++ > CalculatingFrequencyInTicks)
	{
		TicksFromLastCalculate = 0;

		LastCalculatedOutput = ActivateNeuralNetwork();

		NeatOrganism->fitness = EvaluateFitness();
	}
}

double* AAgentController::ActivateNeuralNetwork()
{
	double* Input = new double[11];
	double* Output = new double[4];
	double MaxDistance = 9000.0f;
	Input[0] = Agent->GetDistanceToBall() / MaxDistance;
	Input[1] = Agent->GetDistanceToGoal() / MaxDistance;
	Input[2] = Agent->GetDistanceToTeammates()[0] / MaxDistance;
	Input[3] = Agent->GetDistanceToTeammates()[1] / MaxDistance;
	Input[4] = Agent->GetDistanceToOpponents()[0] / MaxDistance;
	Input[5] = Agent->GetDistanceToOpponents()[1] / MaxDistance;
	Input[6] = Agent->GetDistanceToWalls()[0] / MaxDistance;
	Input[7] = Agent->GetDistanceToWalls()[1] / MaxDistance;
	Input[8] = Agent->GetDistanceToWalls()[2] / MaxDistance;
	Input[9] = Agent->GetDistanceToWalls()[3] / MaxDistance;
	Input[10] = .5;

	//for (int i = 0; i < 10; i++) {
	//	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString::SanitizeFloat(Input[i]));
	//}

	NeatOrganism->net->loadSensors(Input);

	NeatOrganism->net->activate();

	int i = 0;
	std::vector<NEAT::NNode*>::iterator OutputIterator = NeatOrganism->net->outputs.begin();
	for (OutputIterator = NeatOrganism->net->outputs.begin(); OutputIterator != NeatOrganism->net->outputs.end(); OutputIterator++ , i++)
	{
		Output[i] = (*OutputIterator)->activation;
		//GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString::SanitizeFloat((*OutputIterator)->activation));
	}

	return Output;
}

double AAgentController::EvaluateFitness()
{
	double Result = 0.0f;
	double MaxValue = 10000.0f;

	if (LastCalculatedOutput)
	{
		Result = MaxValue - Agent->GetDistanceToGoal();
	}

	return Result;
}

void AAgentController::ControlAgent(double* ControlValues)
{
	if (!ControlValues)
	{
		return;
	}

	int16 moveStep = 5;

	FVector MovementVector = FVector::ZeroVector;

	//if(round(ControlValues[0]) == 1)
	MovementVector += ControlValues[0] * FVector::ForwardVector;

	//if (round(ControlValues[1]) == 1)
	MovementVector -= ControlValues[1] * FVector::ForwardVector;

	//if (round(ControlValues[2]) == 1)
	MovementVector += ControlValues[2] * FVector::RightVector;

	//if (round(ControlValues[3]) == 1)
	MovementVector -= ControlValues[3] * FVector::RightVector;

	Agent->AddMovementInput(MovementVector, moveStep);
}
