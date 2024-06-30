// Fill out your copyright notice in the Description page of Project Settings.


#include "DPWeaponActorComponent.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "DPWeaponGun.h"
#include "FNetLogger.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
//#include "DPWeapon.h"

class ADPWeaponGun;
// Sets default values for this component's properties
UDPWeaponActorComponent::UDPWeaponActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UDPWeaponActorComponent::BeginPlay()
{
	Super::BeginPlay();

	ADPCharacter* playerCharacter = Cast<ADPCharacter>(GetOwner());
	if (playerCharacter) {
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = Cast<AActor>(playerCharacter);
		ADPWeaponGun* WeaponGun = GetWorld()->SpawnActor<ADPWeaponGun>(ADPWeaponGun::StaticClass(), spawnParams);

		if (WeaponGun)
			WeaponGun->AttachToComponent(playerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("gunSocket"));
	}
}


// Called every frame
void UDPWeaponActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UDPWeaponActorComponent::SimulateAttack(ADPCharacter* character, FHitResult& result, const Gunfire& gunfire)
{
	if (currentWeapon)
		return currentWeapon->SimulateAttack(character, result, gunfire);
	return false;
}

void UDPWeaponActorComponent::AddWeapons(TSubclassOf<ADPWeapon> weaponClass)
{
	if (weaponClass) {
		ADPWeapon* newWeapon = NewObject<ADPWeapon>(this, weaponClass);
		if (newWeapon)
			weapons.Add(newWeapon); UE_LOG(LogTemp, Warning, TEXT("add weapon : %s"), *newWeapon->GetName());
	}
}

void UDPWeaponActorComponent::Equip(TSubclassOf<ADPWeapon> weaponClass)
{
	for (ADPWeapon* weapon : weapons) {
		if (weapon->IsA(weaponClass)) {
			currentWeapon = weapon;
			UE_LOG(LogTemp, Warning, TEXT("equip weapon : %s"), *weapon->GetName());
			break;
		}
	}
}

bool UDPWeaponActorComponent::Attack(ADPPlayerController* controller, FHitResult& result, FRotator& info)
{
	if (currentWeapon)
		return currentWeapon->Attack(controller, result, info);
	return false;
}
