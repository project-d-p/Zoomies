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

	virtual void Attack() override;
};
