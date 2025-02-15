#pragma once

#include "AIController.h"
#include "Organism.h"
#include "Agent.h"
#include "Ball.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AgentController.generated.h"

UCLASS()
class CCE_API AAgentController : public AAIController
{
	GENERATED_BODY()
private:
	NEAT::Organism* NeatOrganism;

	AAgent* Agent;

	int8 TicksFromLastCalculate = 0;

	int8 const CalculatingFrequencyInTicks = 60;

	double* LastCalculatedOutput;

	void ControlAgent(double* ControlValues) const;

	double* ActivateNeuralNetwork() const;

public:
	AAgentController();

	virtual void Tick(float DeltaSeconds) override;
	void MoveAgent();

	virtual void Possess(APawn* Pawn) override;

	double* CalcualteOutput();

	double EvaluateFitness() const;

	FORCEINLINE NEAT::Organism* GetNeatOrganism() const { return NeatOrganism; }
	FORCEINLINE void SetNeatOrganism(NEAT::Organism* Value) { NeatOrganism = Value; }
	FORCEINLINE AAgent* GetAgent() const { return Agent; }
};
