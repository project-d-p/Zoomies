#include "WhaleCharacter.h"

#include "Components/CapsuleComponent.h"


AWhaleCharacter::AWhaleCharacter()
{
	InitMonsterMeshData(AnimalType);

	/** Set the Capsule size */
	DefaultCP.Radius = 65.f;
	DefaultCP.HalfHeight = 110.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 65.f;
	FaintCP.HalfHeight = 90.f;
	
	/** Set the model size and adjust position */
	FVector Scale(FVector::OneVector);
	FVector Location(0.f, 0.f, 0.f);
	FRotator Rotation(0.f, 0.f, 0.f);
	FTransform Transform(Rotation, Location, Scale);
		
	MeshAdjMtx = Transform;
	GetMesh()->SetRelativeTransform(MeshAdjMtx);

	/** Set the faint state matrix */
	FaintStateMtx = FTransform(
		FRotator(0.f, 0.f, 90.f),
		FVector::ZeroVector,
		FVector::OneVector);
	CB_FaintStateMtx =  MeshAdjMtx.Inverse() * FaintStateMtx * MeshAdjMtx;
}
