#include "HitScan.h"
#include "DPCharacter.h"

#include "DPWeaponActorComponent.h"

// TODO: Make HitDetection as a Component
bool UHitScan::HitDetect(const ADPCharacter* character, const FVector& start, const FRotator& direction, const float& distance, FHitResult& result, bool bDebugLine)
{
	FVector end = start + direction.Vector() * distance;

	/* 
	 * Debug Line
	 */
	if (bDebugLine)
	{
		UWorld *World = GetWorld();
		//DrawDebugLine(World, start, end, FColor::Blue, false, 4.0f, 0, 5.0f);
	}
	// ------------
	
	FHitResult hit;
	FCollisionQueryParams query_params;
	query_params.AddIgnoredActor(character);
	query_params.AddIgnoredActor(character->weaponComponent->currentWeapon);

	bool bHitSuccess = GetWorld()->LineTraceSingleByChannel(
		hit,
		start,
		end,
		ECC_Visibility,
		query_params
	);
	result = hit;
	return bHitSuccess;
}

UHitScan::~UHitScan()
{
}
