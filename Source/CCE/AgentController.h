#pragma once

#include "AIController.h"
#include "Network.h"
#include "AgentController.generated.h"

UCLASS()
class CCE_API AAgentController : public AAIController {
  GENERATED_BODY()

  UBlackboardComponent *AgentBlackBoardComponent;

  UBehaviorTreeComponent *AgentBehaviorTreeComponent;

  virtual void Possess(APawn *Pawn) override;

  

  NEAT::Network *NeuralNetwork;

public:
  AAgentController();

  FORCEINLINE UBlackboardComponent *GetAgentBlackBoardComponent() const {return AgentBlackBoardComponent;}

  FORCEINLINE UBehaviorTreeComponent *GetAgentBehaviorTreeComponent() const {return AgentBehaviorTreeComponent;}

  FORCEINLINE NEAT::Network *GetNeuralNetwork() const {return NeuralNetwork;}

  FORCEINLINE void SetNeuralNetwork(NEAT::Network *Value) {NeuralNetwork = Value;}

 // void OnPossess(APawn *Pawn);
};
