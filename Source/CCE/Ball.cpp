#include "CCE.h"
#include "Ball.h"


ABall::ABall()
{
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box"));
	RootComponent = mesh;
}

void ABall::BeginPlay()
{
	Super::BeginPlay();
}

void ABall::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

