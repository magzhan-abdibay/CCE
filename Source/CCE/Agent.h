#pragma once

#include <vector>
#include "AgentBase.h"
#include "Agent.generated.h"

UCLASS(config = Game)

class AAgent : public AAgentBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = Team)
	int8 Team;

	float DistanceToBall = 0.0f;
	float DistanceToGoal = 0.0f;
	std::vector<float> DistanceToTeammates;
	std::vector<float> DistanceToOpponents;
	std::vector<float> DistanceToWalls;

	float CalculateDistanceToBall();
	float CalculateDistanceToGoal();
	std::vector<float> CalcualteDistanceToTeammates();
	std::vector<float> CalcualteDistanceToOpponents();
	std::vector<float> CalcualteDistanceToWalls();

	TArray<AActor*> GetClosestAgents();
public:
	AAgent();

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	FORCEINLINE int8 GetTeam() const { return Team; }

	FORCEINLINE void SetTeam(int8 Value) { Team = Value; }

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
};
