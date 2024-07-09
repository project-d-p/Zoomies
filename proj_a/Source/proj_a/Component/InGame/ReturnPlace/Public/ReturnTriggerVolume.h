#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"
#include "ReturnTriggerVolume.generated.h"

UCLASS()
class AReturnTriggerVolume : public AActor
{
	GENERATED_BODY()
public:
	AReturnTriggerVolume();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "effects")
	class UNiagaraSystem* EffectsReturn;
	void SpawnReturnEffect(TArray<EAnimal> Array);
	
protected:
	virtual void BeginPlay() override;

private:
	// Box component to represent the volume
	UPROPERTY(VisibleAnywhere)
	USphereComponent* TriggerSphere;

	// Function to handle the overlap beginning
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Function to handle the overlap ending
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
