#include "CCE.h"
#include "Goal.h"

AGoal::AGoal()
{
	PrimaryActorTick.bCanEverTick = true;
	box= CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	box->bGenerateOverlapEvents = true;
	RootComponent = box;
	box->OnComponentBeginOverlap.AddDynamic(this, &AGoal::TriggerEnter);
	box->OnComponentEndOverlap.AddDynamic(this, &AGoal::TriggerExit);

	light = CreateDefaultSubobject<UPointLightComponent>(TEXT("light"));
	light->Intensity = 10000;
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
	light->SetLightColor(FColor::Green);
}

void AGoal::TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	light->SetLightColor(FColor::Yellow);
}


