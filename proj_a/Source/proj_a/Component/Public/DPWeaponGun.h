// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DPWeapon.h"
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

	virtual bool Attack(ADPPlayerController* controller, FHitResult& result) override;
	virtual bool SimulateAttack(ADPPlayerController* Controller, FHitResult& Result, const Message& Message);
	virtual bool SimulateAttackByClient(ADPCharacter* Character, FHitResult& HitResult, const Gunfire& Gunfire);
};
