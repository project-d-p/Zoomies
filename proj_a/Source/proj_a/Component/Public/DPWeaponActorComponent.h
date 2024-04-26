// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DPWeapon.h"
#include "Components/ActorComponent.h"
#include "DPWeaponActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_A_API UDPWeaponActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDPWeaponActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray<ADPWeapon*> weapons;
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	ADPWeapon* currentWeapon;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AddWeapons(TSubclassOf<ADPWeapon> weaponClass);
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Equip(TSubclassOf<ADPWeapon> weaponClass);
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Attack();
};
