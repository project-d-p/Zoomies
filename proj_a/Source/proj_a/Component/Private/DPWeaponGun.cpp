// Fill out your copyright notice in the Description page of Project Settings.


#include "DPWeaponGun.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "Components/CapsuleComponent.h"
#include "EntitySystem/MovieSceneEntitySystemTypes.h"
#include "GameFramework/Character.h"

ADPWeaponGun::ADPWeaponGun()
{
	damage = 15.f;
}

bool ADPWeaponGun::Attack(ADPPlayerController* controller, FHitResult& result)
{
	FVector start = controller->GetCharacter()->GetActorLocation();
	// start += FVector(0, 0, controller->GetCharacter()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FNetLogger::EditerLog(FColor::Cyan, TEXT("start : %s"), *start.ToString());

	const FRotator aim_direction = controller->GetControlRotation();
	FVector end = start + aim_direction.Vector() * 100000000.f;

	/*
	 * Debug Line
	 */
	UWorld *World = GetWorld();
	DrawDebugLine(World, start, end, FColor::Blue, false, 4.0f, 0, 5.0f);
	// ------------
	
	FHitResult hit;

	FCollisionQueryParams query_params;
	// query_params.AddIgnoredActor(GetOwner());
	query_params.AddIgnoredActor(controller->GetCharacter());
	query_params.AddIgnoredActor(this);

	bool bHitSuccess = GetWorld()->LineTraceSingleByChannel(
		hit,
		start,
		end,
		ECC_Visibility,
		query_params
	);

	if (bHitSuccess)
	{
		result = hit;
		
		ADPCharacter* character_ = Cast<ADPCharacter>(hit.GetActor());
		if (character_ == nullptr)
		{
			FNetLogger::EditerLog(FColor::Cyan, TEXT("hit[not character], %s, %s"), *hit.Location.ToString(), *hit.GetActor()->GetName());
		}
		else
		{
			ADPPlayerState* player_state = Cast<ADPPlayerState>(character_->GetPlayerState());
			FNetLogger::EditerLog(FColor::Cyan, TEXT("hit : %s, %s"), *hit.Location.ToString(), *player_state->GetPlayerName());
		}
	}
	else
	{
		result = hit;
		FNetLogger::EditerLog(FColor::Cyan, TEXT("no hit"));
	}
	return bHitSuccess;
}

bool ADPWeaponGun::SimulateAttack(ADPPlayerController* Controller, FHitResult& Result, const Message& Message)
{
	Gunfire gunfire = Message.gunfire();
	FVector start = FVector(gunfire.position().x(), gunfire.position().y(), gunfire.position().z());

	const FRotator aim_direction = FRotator(gunfire.direction().x(), gunfire.direction().y(), gunfire.direction().z());
	FVector end = start + aim_direction.Vector() * 100000000.f;

	/*
	 * Debug Line
	 */
	UWorld *World = GetWorld();
	DrawDebugLine(World, start, end, FColor::Blue, false, 4.0f, 0, 5.0f);
	// ------------
	
	FCollisionQueryParams query_params;
	// query_params.AddIgnoredActor(GetOwner());
	query_params.AddIgnoredActor(Controller->GetCharacter());
	query_params.AddIgnoredActor(this);

	FHitResult hit;
	bool bHitSuccess = GetWorld()->LineTraceSingleByChannel(
		hit,
		start,
		end,
		ECC_Visibility,
		query_params
	);

	if (bHitSuccess)
	{
		Result = hit;
		
		ADPCharacter* character_ = Cast<ADPCharacter>(hit.GetActor());
		if (character_ == nullptr)
		{
			FNetLogger::EditerLog(FColor::Cyan, TEXT("hit[not character], %s, %s"), *hit.Location.ToString(), *hit.GetActor()->GetName());
		}
		else
		{
			ADPPlayerState* player_state = Cast<ADPPlayerState>(character_->GetPlayerState());
			FNetLogger::EditerLog(FColor::Cyan, TEXT("hit : %s, %s"), *hit.Location.ToString(), *player_state->GetPlayerName());
		}
	}
	else
	{
		Result = hit;
		FNetLogger::EditerLog(FColor::Cyan, TEXT("no hit"));
	}
	return bHitSuccess;
}

bool ADPWeaponGun::SimulateAttackByClient(ADPCharacter* Character, FHitResult& Result, const Gunfire& Gunfire)
{
	FVector start = FVector(Gunfire.position().x(), Gunfire.position().y(), Gunfire.position().z());

	const FRotator aim_direction = FRotator(Gunfire.direction().x(), Gunfire.direction().y(), Gunfire.direction().z());
	FVector end = start + aim_direction.Vector() * 100000000.f;
	FCollisionQueryParams query_params;
	// query_params.AddIgnoredActor(GetOwner());
	query_params.AddIgnoredActor(Character);
	query_params.AddIgnoredActor(this);

	/*
	 * Debug Line
	 */
	UWorld *World = GetWorld();
	DrawDebugLine(World, start, end, FColor::Blue, false, 4.0f, 0, 5.0f);
	// ------------
	
	FHitResult hit;
	bool bHitSuccess = GetWorld()->LineTraceSingleByChannel(
		hit,
		start,
		end,
		ECC_Visibility,
		query_params
	);

	if (bHitSuccess)
	{
		Result = hit;
		
		ADPCharacter* character_ = Cast<ADPCharacter>(hit.GetActor());
		if (character_ == nullptr)
		{
			FNetLogger::EditerLog(FColor::Cyan, TEXT("hit[not character], %s, %s"), *hit.Location.ToString(), *hit.GetActor()->GetName());
		}
		else
		{
			ADPPlayerState* player_state = Cast<ADPPlayerState>(character_->GetPlayerState());
			FNetLogger::EditerLog(FColor::Cyan, TEXT("hit : %s, %s"), *hit.Location.ToString(), *player_state->GetPlayerName());
		}
	}
	else
	{
		Result = hit;
		FNetLogger::EditerLog(FColor::Cyan, TEXT("no hit"));
	}
	return bHitSuccess;
}
