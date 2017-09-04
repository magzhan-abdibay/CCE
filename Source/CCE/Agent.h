#pragma once

#include <vector>
#include "AgentBase.h"
#include "Agent.generated.h"


// + ditance to walls 
// + distance to the ball 
// + number of kicks
// + number of false kicks 
//ball distance to the opponent's goal
// + number of scores

UCLASS(config = Game)

class AAgent : public AAgentBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = Team)
	int8 Team = 0;

	float DistanceToBall = 0.0f;
	float DistanceToGoal = 0.0f;
	std::vector<float> DistanceToTeammates;
	std::vector<float> DistanceToOpponents;
	std::vector<float> DistanceToWalls;
	int ScoredPoints = 0;
	int NumberOfKicks = 0;
	int NumberOfFalseKicks = 0;

	float CalculateDistanceToBall() const;
	float CalculateDistanceToGoal() const;
	std::vector<float> CalcualteDistanceToTeammates();
	std::vector<float> CalcualteDistanceToOpponents();
	std::vector<float> CalcualteDistanceToWalls() const;
	
	TArray<AActor*> GetClosestAgents();
	void DifferentiateTeamsByColor();
public:
	AAgent();

	virtual void Tick(float DeltaSeconds) override;

	void Kick() override;

	FORCEINLINE int8 GetTeam() const { return Team; }

	FORCEINLINE void SetTeam(int8 Value){Team = Value; DifferentiateTeamsByColor();}
	
	FORCEINLINE float GetDistanceToBall() const { return DistanceToBall; }

	FORCEINLINE void SetDistanceToBall(float Value) { DistanceToBall = Value; }

	FORCEINLINE float GetDistanceToGoal() const { return DistanceToGoal; }

	FORCEINLINE void SetDistanceToGoal(float Value) { DistanceToGoal = Value; }

	FORCEINLINE std::vector<float> GetDistanceToTeammates() const { return DistanceToTeammates; }

	FORCEINLINE void SetDistanceToTeammates(std::vector<float> Value) { DistanceToTeammates = Value; }

	FORCEINLINE std::vector<float> GetDistanceToOpponents() const { return DistanceToOpponents; }

	FORCEINLINE void SetDistanceToOpponents(std::vector<float> Value) { DistanceToOpponents = Value; }

	FORCEINLINE std::vector<float> GetDistanceToWalls() const { return DistanceToWalls; }

	FORCEINLINE void SetDistanceToWalls(std::vector<float> Value) { DistanceToWalls = Value; }

	UFUNCTION(BlueprintPure, Category = "Score")
	FORCEINLINE int GetScoredPoints() const { return ScoredPoints; }

	FORCEINLINE void SetScoredPoints(int Value) { ScoredPoints = Value; }

	FORCEINLINE void IncrementScoredPoints() { ScoredPoints++; GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue, FString::FromInt(ScoredPoints));}

	FORCEINLINE void DecrementScoredPoints() { ScoredPoints--; GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue, FString::FromInt(ScoredPoints));}

	UFUNCTION(BlueprintPure, Category = "Score")
	FORCEINLINE int GetNumberOfKicks() const { return NumberOfKicks; }

	FORCEINLINE void SetNumberOfKicks(int Value) { NumberOfKicks = Value; }
	
	UFUNCTION(BlueprintPure, Category = "Score")
	FORCEINLINE int GetNumberOfFalseKicks() const { return NumberOfFalseKicks; }

	FORCEINLINE void SeNumberOfFalseKicks(int Value) { NumberOfFalseKicks = Value; }

};
