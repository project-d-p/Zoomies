// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DPWeapon.h"
#include "DPWeaponRifle.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API ADPWeaponRifle : public ADPWeapon
{
	GENERATED_BODY()

public:
	ADPWeaponRifle();

	virtual void attack() override;
};
