#include "SaberToothTigerCharacter.h"

#include "Components/CapsuleComponent.h"


ASaberToothTigerCharacter::ASaberToothTigerCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SABERTOOTHTIGER
	(TEXT("/Game/model/animals/sabertoothTiger/sm_sabertoothTiger.sm_sabertoothTiger"));
	if (SK_SABERTOOTHTIGER.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_SABERTOOTHTIGER.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER(
		TEXT("/Game/animation/animals/sabertoothTigerAnimation.sabertoothTigerAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	GetCapsuleComponent()->SetCapsuleRadius(90.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(90.f);
	
	/** Set the model size and adjust position */
	FVector Scale(FVector::OneVector);
	FVector Location(0.f, 0.f, 0.f);
	FRotator Rotation(0.f, 0.f, 0.f);
	FTransform Transform(Rotation, Location, Scale);
		
	MeshAdjMtx = Transform;
	GetMesh()->SetRelativeTransform(MeshAdjMtx);

	/** Set the faint state matrix */
	FaintStateMtx = FTransform(
		FRotator(90.f, 0.f, 0.f),
		FVector::ZeroVector,
		FVector::OneVector);
	CB_FaintStateMtx =  MeshAdjMtx.Inverse() * FaintStateMtx * MeshAdjMtx;
}
