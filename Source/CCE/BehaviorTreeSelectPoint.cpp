#include "CCE.h"
#include "BehaviorTreeSelectPoint.h"
#include "AgentController.h"
#include "Engine/TargetPoint.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UBehaviorTreeSelectPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAgentController* Controller = Cast<AAgentController>(OwnerComp.GetAIOwner());
	if(Controller)
	{
		UBlackboardComponent* BlackBoard = Controller->GetAgentBlackBoardComponent();
	
		TArray<AActor*> AvailablePoints = Controller->GetLocationTargets();
		ATargetPoint* LocationToGo;
		int32 CurrentLocationTarget= Controller->GetCurrentLocationTarget();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::FromInt(AvailablePoints.Num()));
		if(CurrentLocationTarget !=AvailablePoints.Num()-1)
		{
			LocationToGo = Cast<ATargetPoint>(AvailablePoints[++CurrentLocationTarget]);
		}else
		{
			CurrentLocationTarget = 0;
			LocationToGo = Cast<ATargetPoint>(AvailablePoints[CurrentLocationTarget]);
		}
		Controller->SetCurrentLocationTarget(CurrentLocationTarget);
		BlackBoard->SetValueAsObject("LocationToGo", LocationToGo);

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
