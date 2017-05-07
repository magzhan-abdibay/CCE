#pragma once

#include "AIController.h"
#include "Network.h"
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
  NEAT::Network *NeuralNetwork;
  
  AAgent *Agent;
  
  int TicksFromLastCalculate = 0;

  double* LastCalculatedOutput;

  void MoveTo(FVector Dest);
public:
  AAgentController();

  virtual void Tick(float DeltaSeconds) override;

  virtual void Possess(APawn *Pawn) override;

  FORCEINLINE NEAT::Network *GetNeuralNetwork() const {return NeuralNetwork;}

  FORCEINLINE void SetNeuralNetwork(NEAT::Network *Value) {NeuralNetwork = Value;}

  FORCEINLINE AAgent *GetAgent() const { return Agent; }

  double* ActivateNeuralNetwork();

  void ControlAgent(double* ControlValues);
};
