#pragma once


#include "CoreMinimal.h"
#include <map>
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "proj_a/Component/InGame/Score/Types/ScoreTypes.h"
#include "ReturnTriggerVolume.generated.h"

UCLASS()
class AReturnTriggerVolume : public AActor
{
	GENERATED_BODY()
public:

	struct FMeshAnimationData
    {
        float TotalTime;
        FTimerHandle AnimationTimerHandle;
        FTimerHandle DestroyTimerHandle;
        int32 Index;
    };
	
	AReturnTriggerVolume();
	virtual ~AReturnTriggerVolume() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "effects")
	class UNiagaraSystem* EffectsReturn;
	void SpawnReturnEffect(TArray<EAnimal> Array);

	UFUNCTION()
	void AnimateAnimalMesh(USkeletalMeshComponent* Mesh);
	void SpawnSingleMonster(EAnimal Animal, int32 INT32);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// Function to handle the overlap beginning
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Function to handle the overlap ending
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void InitializeMonsterMeshes();

	// Box component to represent the volume
	UPROPERTY(VisibleAnywhere)
	USphereComponent* TriggerSphere;

	std::map<EAnimal, USkeletalMesh*> monsterMeshMap;

	std::map<USkeletalMeshComponent*, FMeshAnimationData> MeshAnimationMap;
	UPROPERTY()
	TArray<FTimerHandle> SpawnTimerHandles;
};
