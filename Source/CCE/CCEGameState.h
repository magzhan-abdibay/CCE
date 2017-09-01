#pragma once

#include "GameFramework/GameState.h"
#include "CCEGameState.generated.h"

UCLASS()
class CCE_API ACCEGameState : public AGameState {
	GENERATED_BODY()
public:
	ACCEGameState();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScoreTeam0() const { return ScoreTeam0; }

	FORCEINLINE void SetScoreTeam0(int32 Value) { ScoreTeam0 = Value; }

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScoreTeam1() const { return ScoreTeam1; }

	FORCEINLINE void SetScoreTeam1(int32 Value) { ScoreTeam1 = Value; }

private:
	int32 ScoreTeam0;

	int32 ScoreTeam1;
};
