// Fill out your copyright notice in the Description page of Project Settings.


#include "DPWeaponActorComponent.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "DPWeaponGun.h"
#include "FNetLogger.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/MatchingLobby/CHAR_MatchingLobby/CHAR_MatchingLobby.h"
//#include "DPWeapon.h"

class ADPWeaponGun;
// Sets default values for this component's properties
UDPWeaponActorComponent::UDPWeaponActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

UDPWeaponActorComponent::~UDPWeaponActorComponent()
{
	// if (weapons.Num() > 0) {
	// 	for (ADPWeapon* weapon : weapons) {
	// 		weapon->Destroy();
	// 	}
	// }
}


// Called when the game starts
void UDPWeaponActorComponent::BeginPlay()
{
	Super::BeginPlay();

	TSubclassOf<ADPWeapon> gunClass = ADPWeaponGun::StaticClass();
	if (UWorld* World = GetWorld())
	{
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World);
		
		if (CurrentLevelName == "matchLobby")
		{
			this->AddWeapons_MatchLobby(gunClass);
		}
		else
		{
			this->AddWeapons(gunClass);
		}
	}
	this->Equip(gunClass);
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

FVector UDPWeaponActorComponent::GetFireLocation()
{
	if (currentWeapon)
		return currentWeapon->GetFireLocation();
	return FVector();
}

void UDPWeaponActorComponent::SpawnEffects(const FHitResult& HitResult, const FRotator& Rotator)
{
	if (currentWeapon)
		currentWeapon->SpawnEffects(HitResult, Rotator);
}

void UDPWeaponActorComponent::AddWeapons(TSubclassOf<ADPWeapon> weaponClass)
{
	ADPCharacter* playerCharacter = Cast<ADPCharacter>(GetOwner());
	if (weaponClass) {
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = Cast<AActor>(playerCharacter);
		ADPWeapon* newWeapon = GetWorld()->SpawnActor<ADPWeaponGun>(ADPWeaponGun::StaticClass(), spawnParams);
		if (newWeapon)
		{
			weapons.Add(newWeapon); UE_LOG(LogTemp, Warning, TEXT("add weapon : %s"), *newWeapon->GetName());
			newWeapon->AttachToComponent(playerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("gunSocket"));
		}
	}
}

void UDPWeaponActorComponent::AddWeapons_MatchLobby(TSubclassOf<ADPWeapon> weaponClass)
{
	ACHAR_MatchingLobby* playerCharacter = Cast<ACHAR_MatchingLobby>(GetOwner());
	if (weaponClass) {
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = Cast<AActor>(playerCharacter);
		ADPWeapon* newWeapon = GetWorld()->SpawnActor<ADPWeaponGun>(ADPWeaponGun::StaticClass(), spawnParams);
		if (newWeapon)
		{
			weapons.Add(newWeapon); UE_LOG(LogTemp, Warning, TEXT("add weapon : %s"), *newWeapon->GetName());
			newWeapon->AttachToComponent(playerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("gunSocket"));
		}
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
