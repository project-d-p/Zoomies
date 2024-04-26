// Fill out your copyright notice in the Description page of Project Settings.


#include "DPWeaponActorComponent.h"
//#include "DPWeapon.h"

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

}


// Called every frame
void UDPWeaponActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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

void UDPWeaponActorComponent::Attack()
{
	if (currentWeapon)
		currentWeapon->Attack();	UE_LOG(LogTemp, Warning, TEXT("attack weaponActorComponent"));
	UE_LOG(LogTemp, Warning, TEXT("current weapon : %s"), *currentWeapon->GetName());
}

