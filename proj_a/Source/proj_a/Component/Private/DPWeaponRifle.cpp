// Fill out your copyright notice in the Description page of Project Settings.


#include "DPWeaponRifle.h"
#include "DPCharacter.h"

ADPWeaponRifle::ADPWeaponRifle()
{
	damage = 15.f;
}

void ADPWeaponRifle::Attack()
{
	// ถ๓ภฬวร
	UE_LOG(LogTemp, Warning, TEXT("attack weapon Rifle"));
	
	if (!character) {
		character = Cast<ADPCharacter>(GetOwner());
		UE_LOG(LogTemp, Warning, TEXT("character get owner"));
	}
	
	if (character) {
		character->PlayFireAnimation();
		UE_LOG(LogTemp, Warning, TEXT("have owner"));
	}
}
