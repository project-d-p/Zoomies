// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DPWeapon.h"
#include "Components/ArrowComponent.h"
#include "DPWeaponGun.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API ADPWeaponGun : public ADPWeapon
{
	GENERATED_BODY()
	
public:
	ADPWeaponGun();
	virtual void BeginPlay() override;

	virtual bool Attack(ADPPlayerController* character, FHitResult& result, FRotator& info) override;
	virtual bool SimulateAttack(ADPCharacter* character, FHitResult& result, const Gunfire& gunfire) override;
	virtual void SpawnEffects(const FHitResult& localtion, const FRotator& rotation) override;
	virtual FVector GetFireLocation() override;

protected:
	UPROPERTY()
	UHitScan* hitScan;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* gunMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MuzzleArrow")
	UArrowComponent* muzzle;
	
	// effect
	//UPROPERTY(VisibleAnywhere)
	//UArrowComponent* arrowSparkle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "effects")
	class UNiagaraSystem* trailEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "effects")
	class UNiagaraSystem* smokeEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "effects")
	class UNiagaraSystem* particleEffect;
	
private:
	void SpawnHitEffect(const FHitResult& HitResult);
};
