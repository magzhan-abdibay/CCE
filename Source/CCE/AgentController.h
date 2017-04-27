#pragma once

#include "AIController.h"
#include "AgentController.generated.h"

UCLASS()
class CCE_API AAgentController : public AAIController {
  GENERATED_BODY()

  UBlackboardComponent *AgentBlackBoardComponent;

  UBehaviorTreeComponent *AgentBehaviorTreeComponent;

  virtual void Possess(APawn *Pawn) override;

public:
  AAgentController();

  FORCEINLINE UBlackboardComponent *GetAgentBlackBoardComponent() const {
    return AgentBlackBoardComponent;
  }

  FORCEINLINE UBehaviorTreeComponent *GetAgentBehaviorTreeComponent() const {
    return AgentBehaviorTreeComponent;
  }
};
