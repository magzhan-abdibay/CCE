#pragma once

#include "GameFramework/Actor.h"
#include "Agent.h"
#include "Ball.generated.h"

UCLASS()
class CCE_API ABall : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Mesh)
	
	class UStaticMeshComponent* Mesh;

	class AAgent* LastKickedAgent = nullptr;
public:
	ABall();

	virtual void BeginPlay() override;

	FORCEINLINE void SetLastKickedAgent(class AAgent* Value) { LastKickedAgent = Value; }

	FORCEINLINE class AAgent* GetLastKickedAgent() const { return LastKickedAgent; }
};
