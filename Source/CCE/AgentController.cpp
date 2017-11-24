#include "CCE.h"
#include "AgentController.h"

AAgentController::AAgentController()
{}


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
		Agent->SetFitnessValue(NeatOrganism->fitness);
	}
}

double* AAgentController::ActivateNeuralNetwork() const
{
	double* Input = new double[11];
	double* Output = new double[5];
	double MaxDistance = 9000.0f;
	double MaxAngle = 360.0f;
	double MaxDistanceToObstacle = 300.0f;
//	Input[0] = Agent->GetDistanceToBall() / MaxDistance;
//	Input[1] = Agent->GetAngleToBall() / MaxAngle;
//	Input[2] = Agent->GetDistanceToGoal() / MaxDistance;
//	Input[3] = Agent->GetDistanceToForwardObstacle() / MaxDistanceToObstacle;
//	if (!Agent->GetDistanceToTeammates().empty())
//	{
//		Input[4] = Agent->GetDistanceToTeammates()[0] != NULL ? Agent->GetDistanceToTeammates()[0] / MaxDistance : 1;
//	}
//	else
//	{
//		Input[4] = 1;
//	}
//	if (!Agent->GetDistanceToOpponents().empty())
//	{
//		Input[5] = Agent->GetDistanceToOpponents()[0] != NULL ? Agent->GetDistanceToOpponents()[0] / MaxDistance : 1;
//	}
//	else
//	{
//		Input[5] = 1;
//	}
//
//	if (!Agent->GetDistanceCircular().empty())
//	{
//		Input[6] = Agent->GetDistanceCircular()[0] != NULL ? Agent->GetDistanceCircular()[0] / MaxDistance : 1;
//		Input[7] = Agent->GetDistanceCircular()[1] != NULL ? Agent->GetDistanceCircular()[1] / MaxDistance : 1;
//		Input[8] = Agent->GetDistanceCircular()[2] != NULL ? Agent->GetDistanceCircular()[2] / MaxDistance : 1;
//		Input[9] = Agent->GetDistanceCircular()[3] != NULL ? Agent->GetDistanceCircular()[3] / MaxDistance : 1;
//	}else
//	{
//		Input[6] = 1;
//		Input[7] = 1;
//		Input[8] = 1;
//		Input[9] = 1;
//	}
	float RayCastLenght = 5000.0f;
	if (!Agent->GetDistanceCircular().empty())
	{
		for (int i = 0; i <= 8; i++)
		{
			Input[i] =  Agent->GetDistanceCircular()[i] / RayCastLenght;
		}
	}else
	{
		for (int i = 0; i <= 8; i++)
		{
			Input[i] = 0;
		}
	}
	Input[9] = 1;
	Input[10] = .5;

	NeatOrganism->net->loadSensors(Input);

	NeatOrganism->net->activate();

	int i = 0;
	std::vector<NEAT::NNode*>::iterator OutputIterator = NeatOrganism->net->outputs.begin();
	for (OutputIterator = NeatOrganism->net->outputs.begin(); OutputIterator != NeatOrganism->net->outputs.end(); ++OutputIterator , i++)
	{
		Output[i] = (*OutputIterator)->activation;
		//GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString::SanitizeFloat((*OutputIterator)->activation));
	}

	return Output;
}

double AAgentController::EvaluateFitness() const
{
	double Result = 0.0f;
	double MaxValue = 10000.0f;

	if (LastCalculatedOutput)
	{
		float MinDistabceToWalls = 100.0f;
	

//		Result = 1000 * Agent->GetNumberOfKicks() - 10 * Agent->GetNumberOfFalseKicks() + 100000 * Agent->GetScoredPoints() + 
//			((MaxValue - (Agent->GetDistanceToBall())) / 10)
//			+ (Agent->GetDistanceToOpponents()[0] + Agent->GetDistanceToOpponents()[1]) / 1000
//			+ ((MaxValue - Agent->GetDistanceToTeammates()[0]) + (MaxValue - Agent->GetDistanceToTeammates()[1])) / 1000
//			- StuckNearWallsPenalty;
//		Result = Result < 0 ? 0 : Result;

//		Result = MaxValue -(Agent->GetDistanceToBall()*cosf(Agent->GetAngleToBall()));
//		Result = Agent->GetAgeInTicks();
//		Result = Agent->GetTotalDistanceTravelled();
//		for (int i = 0; i <= 8; i++)
//		{
//			if (!Agent->GetDistanceCircular().empty()) {
//				if (Agent->GetDistanceCircular()[i] < MinDistabceToWalls)
//				{
//					Result = (1.0f - Agent->GetDistanceToForwardObstacle() / MinDistabceToWalls)*Result;
//					Agent->SetTotalDistanceTravelled(0);
//				}
//			}
//			
//		}

		Result = 0;
		if (!Agent->GetDistanceCircular().empty())
		{
			for (int i = 0; i <= 8; i++)
			{
				Result += Agent->GetDistanceCircular()[i];
			}
		}

		for (int i = 0; i <= 8; i++)
		{
			if (!Agent->GetDistanceCircular().empty())
			{
				if (Agent->GetDistanceCircular()[i] < MinDistabceToWalls)
				{
					Result = Agent->GetDistanceCircular()[i];
				}
			}
		}

//		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString::SanitizeFloat((StuckNearWallsPenalty)));
	}

	return Result;
}

void AAgentController::ControlAgent(double* ControlValues) const
{
	if (!ControlValues)
	{
		return;
	}

	int16 moveStep = 5;
	FVector MovementVector = FVector::ZeroVector;
	MovementVector += ControlValues[0] * FVector::ForwardVector;
	MovementVector -= ControlValues[1] * FVector::ForwardVector;
	MovementVector += ControlValues[2] * FVector::RightVector;
	MovementVector -= ControlValues[3] * FVector::RightVector;
	MovementVector += .1f*Agent->GetActorForwardVector();
	Agent->AddMovementInput(MovementVector, moveStep);

	if (round(ControlValues[4]) == 1)
	{
		Agent->Kick();
	}
}
