#include "CCE.h"
#include "Agent.h"
#include "Ball.h"
#include "Goal.h"

AAgent::AAgent() {
  // Set size for collision capsule
  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

  // Don't rotate when the controller rotates. Let that just affect the camera.
  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  // Configure character movement
  GetCharacterMovement()->bOrientRotationToMovement =
      true; // Character moves in the direction of input...
  GetCharacterMovement()->RotationRate =
      FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
  GetCharacterMovement()->JumpZVelocity = 600.f;
  GetCharacterMovement()->AirControl = 0.2f;

  // Create a camera boom (pulls in towards the player if there is a collision)
  CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
  CameraBoom->SetupAttachment(RootComponent);
  CameraBoom->TargetArmLength =
      300.0f; // The camera follows at this distance behind the character
  CameraBoom->bUsePawnControlRotation =
      true; // Rotate the arm based on the controller

  // Create a follow camera
  FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
  FollowCamera->SetupAttachment(
      CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the
                                                    // end of the boom and let
                                                    // the boom adjust to match
                                                    // the controller
                                                    // orientation
  FollowCamera->bUsePawnControlRotation =
      false; // Camera does not rotate relative to arm
}

void AAgent::SetupPlayerInputComponent(
    class UInputComponent *PlayerInputComponent) {
  // Set up gameplay key bindings
  check(PlayerInputComponent);
  PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
  PlayerInputComponent->BindAction("Jump", IE_Released, this,
                                   &ACharacter::StopJumping);

  PlayerInputComponent->BindAxis("MoveForward", this, &AAgent::MoveForward);
  PlayerInputComponent->BindAxis("MoveRight", this, &AAgent::MoveRight);

  // Rotation bindings
  PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
  PlayerInputComponent->BindAxis("LookUp", this,
                                 &APawn::AddControllerPitchInput);
}

void AAgent::MoveForward(float Value) {
  if ((Controller != nullptr) && (Value != 0.0f)) {
    // find out which way is forward
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    // get forward vector
    const FVector Direction =
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(Direction, Value);
  }
}

void AAgent::MoveRight(float Value) {
  if ((Controller != nullptr) && (Value != 0.0f)) {
    // find out which way is right
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    // get right vector
    const FVector Direction =
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    // add movement in that direction
    AddMovementInput(Direction, Value);
  }
}

void AAgent::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  DistanceToBall = CalculateDistanceToBall();
  //GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red,
  //                                 FString::SanitizeFloat(DistanceToBall));

  DistanceToTeammate = CalcualteDistanceToTeammate();
  //GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan,
  //                                 FString::SanitizeFloat(DistanceToTeammate));

  DistanceToGoal = CalculateDistanceToGoal();
  //GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan,
  //                                 FString::SanitizeFloat(DistanceToGoal));
}

float AAgent::CalculateDistanceToBall() {
  for (TActorIterator<ABall> BallItr(GetWorld()); BallItr; ++BallItr) {
    FVector LinkStart = GetActorLocation();
    FVector LinkEnd = BallItr->GetActorLocation();
    DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Green, false, -1, 0,
                  2.0f);
    return (LinkEnd - LinkStart).Size();
  }
  return 0.0f;
}

float AAgent::CalcualteDistanceToTeammate() {
  for (TActorIterator<AAgent> AgentItr(GetWorld()); AgentItr; ++AgentItr) {
    if (*AgentItr != this && AgentItr->GetTeam() == this->Team) {
      FVector LinkStart = GetActorLocation();
      FVector LinkEnd = AgentItr->GetActorLocation();
      DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Red, false, -1, 1,
                    2.0f);
      return (LinkEnd - LinkStart).Size();
    }
  }
  return 0.0f;
}

float AAgent::CalculateDistanceToGoal() {
  for (TActorIterator<AGoal> GoalItr(GetWorld()); GoalItr; ++GoalItr) {
    if (GoalItr->GetTeam() != this->Team) {
      FVector LinkStart = GetActorLocation();
      FVector LinkEnd = GoalItr->GetActorLocation();
      DrawDebugLine(GetWorld(), LinkStart, LinkEnd, FColor::Cyan, false, -1, 1,
                    2.0f);
      // GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green,
      // FString::SanitizeFloat((LinkEnd - LinkStart).Size()));
      return (LinkEnd - LinkStart).Size();
    }
  }
  return 0.0f;
}
