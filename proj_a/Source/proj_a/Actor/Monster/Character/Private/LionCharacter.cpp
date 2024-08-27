#include "LionCharacter.h"

#include "Components/CapsuleComponent.h"


ALionCharacter::ALionCharacter()
{
	InitMonsterMeshData(AnimalType);

	/** Set the Capsule size */
	DefaultCP.Radius = 74.f;
	DefaultCP.HalfHeight = 127.f;
	GetCapsuleComponent()->SetCapsuleRadius(DefaultCP.Radius);
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCP.HalfHeight);

	FaintCP.Radius = 74.f;
	FaintCP.HalfHeight = 74.f;
	
	/** Set the model size and adjust position */
	FVector Location(0.f, 0.f, 10.f);
	FRotator Rotation(0.f, 0.f, 0.f);
	FTransform Transform(Rotation, Location);
		
	MeshAdjMtx = Transform;
	GetMesh()->SetRelativeTransform(MeshAdjMtx);

	/** Set the faint state matrix */
	FaintStateMtx = FTransform(
		FRotator(0.f, 0.f, 90.f)
		, FVector::ZeroVector);
	CB_FaintStateMtx =  FaintStateMtx;
}