#pragma once

#include "GameFramework/Actor.h"
#include "Ball.generated.h"

UCLASS()
class CCE_API ABall : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Mesh)
	class UStaticMeshComponent * mesh;
	
public:	
	ABall();

	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;


};
