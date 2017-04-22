#include "CCE.h"
#include "Goal.h"
#include "Ball.h"
#include "CCEGameState.h"
#include "DrawDebugHelpers.h"

AGoal::AGoal()
{
	PrimaryActorTick.bCanEverTick = true;
	box= CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	box->bGenerateOverlapEvents = true;
	RootComponent = box;
	box->OnComponentBeginOverlap.AddDynamic(this, &AGoal::TriggerEnter);
	box->OnComponentEndOverlap.AddDynamic(this, &AGoal::TriggerExit);

	light = CreateDefaultSubobject<UPointLightComponent>(TEXT("light"));
	light->Intensity = 100;
	light->SetLightColor(FColor::Red);
	light->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
}

void AGoal::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGoal::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AGoal::TriggerEnter(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(ABall::StaticClass())) {
		light->SetLightColor(FColor::Green);
		ACCEGameState* const GameState = GetWorld() != NULL ? GetWorld()->GetGameState<ACCEGameState>() : NULL;
		if (GameState != NULL) {
			if (Team == 0) {
				GameState->SetScoreTeam1(GameState->GetScoreTeam1() + 1);
			} else if (Team == 1) {
				GameState->SetScoreTeam0(GameState->GetScoreTeam0() + 1);
			}
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue, FString::FromInt(GameState->GetScoreTeam0()));
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue, FString::FromInt(GameState->GetScoreTeam1()));
		}
	}
}

void AGoal::TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ABall::StaticClass())) {
		light->SetLightColor(FColor::Yellow);
	}
}


