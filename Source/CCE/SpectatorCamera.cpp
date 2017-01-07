// Fill out your copyright notice in the Description page of Project Settings.

#include "CCE.h"
#include "SpectatorCamera.h"

// Sets default values
ASpectatorCamera::ASpectatorCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create our components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArmComponent"));
	CameraSpringArmComponent->SetupAttachment(RootComponent);
	CameraSpringArmComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
	CameraSpringArmComponent->TargetArmLength = 400.f;
	CameraSpringArmComponent->bEnableCameraLag = true;
	CameraSpringArmComponent->CameraLagSpeed = 3.0f;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraSpringArmComponent, USpringArmComponent::SocketName);

}

// Called when the game starts or when spawned
void ASpectatorCamera::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpectatorCamera::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	HandleYaw();
	HandlePitch();
	HandleMovement(DeltaTime);
}

void ASpectatorCamera::HandleMovement(float DeltaTime)
{
	//Handle movement based on our "MoveX" and "MoveY" axes
	if (!MovementInput.IsZero())
	{
		//Scale our movement input axis values by cameraSpeed units per second
		MovementInput = MovementInput.GetSafeNormal() * cameraSpeed;
		FVector NewLocation = GetActorLocation();
		NewLocation += GetActorForwardVector() * MovementInput.X * DeltaTime;
		NewLocation += GetActorRightVector() * MovementInput.Y * DeltaTime;
		SetActorLocation(NewLocation);
	}
}

void ASpectatorCamera::HandleYaw()
{
	//Rotate our actor's yaw, which will turn our camera because we're attached to it
    FRotator NewRotation = GetActorRotation();
    NewRotation.Yaw += CameraInput.X;
    SetActorRotation(NewRotation);
}

void ASpectatorCamera::HandlePitch()
{
	//Rotate our camera's pitch, but limit it so we're always looking downward
	FRotator NewRotation = CameraSpringArmComponent->GetComponentRotation();
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + CameraInput.Y, -80.0f, 80.0f);
	CameraSpringArmComponent->SetWorldRotation(NewRotation);
}

// Called to bind functionality to input
void ASpectatorCamera::SetupPlayerInputComponent(class UInputComponent* Value)
{
	Super::SetupPlayerInputComponent(Value);

	//Hook up every-frame handling for our four axes
	Value->BindAxis("MoveForward", this, &ASpectatorCamera::MoveForward);
	Value->BindAxis("MoveRight", this, &ASpectatorCamera::MoveRight);
	Value->BindAxis("CameraPitch", this, &ASpectatorCamera::PitchCamera);
	Value->BindAxis("CameraYaw", this, &ASpectatorCamera::YawCamera);

}

void ASpectatorCamera::MoveForward(float Value)
{
	MovementInput.X = FMath::Clamp<float>(Value, -1.0f, 1.0f);
}

void ASpectatorCamera::MoveRight(float Value)
{
	MovementInput.Y = FMath::Clamp<float>(Value, -1.0f, 1.0f);
}

void ASpectatorCamera::PitchCamera(float Value)
{
	CameraInput.Y = Value;
}

void ASpectatorCamera::YawCamera(float Value)
{
	CameraInput.X = Value;
}