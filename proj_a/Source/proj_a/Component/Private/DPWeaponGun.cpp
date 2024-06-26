// Fill out your copyright notice in the Description page of Project Settings.


#include "DPWeaponGun.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "HitScan.h"
#include "Components/CapsuleComponent.h"
#include "EntitySystem/MovieSceneEntitySystemTypes.h"
#include "GameFramework/Character.h"

ADPWeaponGun::ADPWeaponGun()
{
	damage = 15.f;
	hitScan = CreateDefaultSubobject<UHitScan>(TEXT("HitScan"));
}

bool ADPWeaponGun::Attack(ADPPlayerController* controller, FHitResult& result)
{
	const FVector start = controller->GetCharacter()->GetActorLocation();
	const FRotator aim_direction = controller->GetControlRotation();
	return hitScan->HitDetect(Cast<ADPCharacter>(controller->GetCharacter()), start, aim_direction, 100000000.f, result);
}

bool ADPWeaponGun::SimulateAttack(ADPCharacter* character, FHitResult& result, const Gunfire& gunfire)
{
	const FVector start = FVector(gunfire.position().x(), gunfire.position().y(), gunfire.position().z());
	const FRotator aim_direction = FRotator(gunfire.direction().x(), gunfire.direction().y(), gunfire.direction().z());
	return hitScan->HitDetect(character, start, aim_direction, 100000000.f, result);
}
