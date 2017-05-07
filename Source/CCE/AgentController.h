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

  virtual void Possess(APawn *Pawn) override;

  NEAT::Network *NeuralNetwork;
  
  AAgent *Agent;
  
  void MoveTo(FVector Dest);

  FVector GenerateTargetVector(FVector &value);

  int TicksFromLastCalculate = 0;
public:
  AAgentController();

  FORCEINLINE NEAT::Network *GetNeuralNetwork() const {return NeuralNetwork;}

  FORCEINLINE void SetNeuralNetwork(NEAT::Network *Value) {NeuralNetwork = Value;}

  FORCEINLINE AAgent *GetAgent() const { return Agent; }

  virtual void Tick(float DeltaSeconds) override;

  double DoStuff();
};
