#include "CCE.h"
#include "CCEGameMode.h"
#include "UserWidget.h"

void ACCEGameMode::BeginPlay()
{
	Super::BeginPlay();

	if(ScoreWidgetClass!=nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), ScoreWidgetClass);
		if(CurrentWidget!=nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

ACCEGameMode::ACCEGameMode(): CurrentWidget(nullptr)
{
	//	DefaultPawnClass = AAgent::StaticClass();
	//	GameStateClass = ACCEGameState::StaticClass();
}
