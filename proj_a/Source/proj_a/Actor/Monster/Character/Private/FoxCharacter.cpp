#include "FoxCharacter.h"

#include "Components/CapsuleComponent.h"


AFoxCharacter::AFoxCharacter()
{
	InitMonsterMeshData(AnimalType);

	/** Set the Capsule size */
	DefaultCP.Radius = 72.f;
	DefaultCP.HalfHeight = 150.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 52.f;
	FaintCP.HalfHeight = 52.f;
	
	/** Set the model size and adjust position */
	FVector Location(0.f, 0.f, 10.f);
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
