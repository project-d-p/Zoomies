#include "CrabCharacter.h"

#include "Components/CapsuleComponent.h"

ACrabCharacter::ACrabCharacter()
{
	InitMonsterMeshData(AnimalType);

	/** Set the Capsule size */
	DefaultCP.Radius = 91.f;
	DefaultCP.HalfHeight = 91.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 91.f;
	FaintCP.HalfHeight = 91.f;

	/** Set the model size and adjust position */
	FVector Location(FVector::ZeroVector);
	FQuat Rotation = FQuat(FRotator(0.f, 90.f, 0.f));
	GetMesh()->SetRelativeLocationAndRotation(Location, Rotation);
	
	FTransform AdjMtx = FTransform(
		FQuat(FRotator(0.f, 0.f, 0.f)),
		Location);

	/** Set the faint state matrix */
	FaintStateMtx = FTransform(
		FQuat(FRotator(90.f, 90.f, 0.f)),
		FVector::ZeroVector);
	CB_FaintStateMtx = AdjMtx.Inverse() * FaintStateMtx * AdjMtx;
}
