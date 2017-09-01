#pragma once
#include "GameFramework/GameMode.h"
#include "CCEGameMode.generated.h"

UCLASS()
class ACCEGameMode : public AGameMode
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	ACCEGameMode();

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Score",Meta=(BlueprintProtected="true"))
	TSubclassOf<class UUserWidget> ScoreWidgetClass;

	UPROPERTY()
	class UUserWidget* CurrentWidget;
};



