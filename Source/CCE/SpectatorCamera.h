#pragma once

#include "GameFramework/Pawn.h"
#include "SpectatorCamera.generated.h"

UCLASS()

class CCE_API ASpectatorCamera : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float cameraSpeed = 1500.0f;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* CameraSpringArmComponent;

	UCameraComponent* CameraComponent;
public:
	// Sets default values for this pawn's properties
	ASpectatorCamera();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* Value) override;

private:

	FVector2D MovementInput;
	FVector2D CameraInput;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void PitchCamera(float Value);
	void YawCamera(float Value);

	void HandleMovement(float DeltaTime);
	void HandleYaw();
	void HandlePitch();
};
