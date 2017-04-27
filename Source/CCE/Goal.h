#pragma once

#include "GameFramework/Actor.h"
#include "UObjectGlobals.h"
#include "Goal.generated.h"

UCLASS()
class CCE_API AGoal : public AActor {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = "Box")
  class UBoxComponent *Box;

  UPROPERTY(EditAnywhere, Category = "Light")
  class UPointLightComponent *Light;

  UPROPERTY(EditAnywhere, Category = "Team")
  int8 Team;

  UPROPERTY(EditAnywhere, Category = "Ball")
  TSubclassOf<class ABall> Ball;

public:
  AGoal();

  virtual void BeginPlay() override;

  virtual void Tick(float DeltaSeconds) override;

  FORCEINLINE int8 GetTeam() const { return Team; }
  FORCEINLINE void SetTeam(int8 Value) { Team = Value; }

private:
  UFUNCTION()
  void TriggerEnter(class UPrimitiveComponent *HitComp,
                    class AActor *OtherActor,
                    class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex,
                    bool bFromSweep, const FHitResult &SweepResult);

  UFUNCTION()
  void TriggerExit(class UPrimitiveComponent *HitComp, class AActor *OtherActor,
                   class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
};
