#include "SquidCharacter.h"

#include "Components/CapsuleComponent.h"


ASquidCharacter::ASquidCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_SQUID
	(TEXT("/Game/model/animals/squid/Squid1.Squid1"));
	if (SK_SQUID.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_SQUID.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER(
		TEXT("/Game/animation/animals/squidAnimation.squidAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	GetCapsuleComponent()->SetCapsuleRadius(90.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(90.f);
	
	/** Set the model size and adjust position */
	GetMesh()->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -87.f), FRotator(0.f, 90.f, 0.f));
}
