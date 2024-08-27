#include "DolphinCharacter.h"

#include "Components/CapsuleComponent.h"


ADolphinCharacter::ADolphinCharacter()
{
	InitMonsterMeshData(AnimalType);
	
	/** Set the Capsule size */
	DefaultCP.Radius = 61.f;
	DefaultCP.HalfHeight = 133.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 61.f;
	FaintCP.HalfHeight = 61.f;

	/** Set the model size and adjust position */
	FVector Location(0.f, 0.f, 20.f);
	FRotator Rotation(0.f, 0.f, 0.f);
	FTransform Transform(Rotation, Location);
		
	MeshAdjMtx = Transform;
	GetMesh()->SetRelativeTransform(MeshAdjMtx);

	/** Set the faint state matrix */
	FaintStateMtx = FTransform(
		FRotator(0.f, 0.f, 90.f),
		FVector::ZeroVector);
	CB_FaintStateMtx =  MeshAdjMtx.Inverse() * FaintStateMtx * MeshAdjMtx;
}
