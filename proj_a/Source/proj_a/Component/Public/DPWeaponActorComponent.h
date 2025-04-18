// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterPositionSync.h"
#include "DPPlayerController.h"
#include "DPWeapon.h"
#include "Components/ActorComponent.h"
#include "message.pb.h"
#include "DPWeaponActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJ_A_API UDPWeaponActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDPWeaponActorComponent();
	virtual ~UDPWeaponActorComponent() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool SimulateAttack(ADPCharacter* character, FHitResult& result, const Gunfire& gunfire);
	FVector GetFireLocation();
	void SpawnEffects(const FHitResult& HitResult, const FRotator& Rotator);

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
	bool Attack(ADPPlayerController* controller, FHitResult& result, FRotator& info);
};
