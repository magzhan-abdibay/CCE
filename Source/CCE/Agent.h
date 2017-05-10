#pragma once

#include "GameFramework/Character.h"
#include "Agent.generated.h"

UCLASS(config = Game)
class AAgent : public ACharacter {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = Team)
  int8 Team;

public:
  AAgent();

  virtual void Tick(float DeltaSeconds) override;

  FORCEINLINE int8 GetTeam() const { return Team; }

  FORCEINLINE void SetTeam(int8 Value) { Team = Value; }

  FORCEINLINE float GetDistanceToBall() const { return DistanceToBall; }

  FORCEINLINE void SetDistanceToBall(float Value) { DistanceToBall = Value; }

  FORCEINLINE float GetDistanceToGoal() const { return DistanceToGoal; }

  FORCEINLINE void SetDistanceToGoal(float Value) { DistanceToGoal = Value; }

  FORCEINLINE float GetDistanceToTeammate() const { return DistanceToTeammate; }

  FORCEINLINE void SetDistanceToTeammate(float Value) {DistanceToTeammate = Value;}

private:
  float DistanceToBall = 0.0f;
  float DistanceToGoal = 0.0f;
  float DistanceToTeammate = 0.0f;

  float CalculateDistanceToBall();
  float CalculateDistanceToGoal();
  float CalcualteDistanceToTeammate();
};
