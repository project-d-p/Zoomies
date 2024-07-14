#include "LionCharacter.h"

#include "Components/CapsuleComponent.h"


ALionCharacter::ALionCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_LION
	(PathManager::GetPath(EMonster::LION));
	if (SK_LION.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_LION.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/animals/lionAnimation.lionAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

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