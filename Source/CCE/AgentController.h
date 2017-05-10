#pragma once

#include "AIController.h"
#include "Organism.h"
#include "Agent.h"
#include "AI/Navigation/NavigationPath.h"
#include "Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Engine/TargetPoint.h"
#include "AgentController.generated.h"

UCLASS()
class CCE_API AAgentController : public AAIController {
  GENERATED_BODY()
private:
	NEAT::Organism *NeatOrganism;

	AAgent *Agent;

	int TicksFromLastCalculate = 0;

	double* LastCalculatedOutput;

	void ControlAgent(double* ControlValues);

	double* ActivateNeuralNetwork();

public:
	AAgentController();

	virtual void Tick(float DeltaSeconds) override;

	virtual void Possess(APawn *Pawn) override;

	double EvaluateFitness();

	FORCEINLINE NEAT::Organism *GetNeatOrganism() const { return NeatOrganism; }

	FORCEINLINE void SetNeatOrganism(NEAT::Organism *Value) { NeatOrganism = Value; }

	FORCEINLINE AAgent *GetAgent() const { return Agent; }

};
