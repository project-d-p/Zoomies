#include "CrabCharacter.h"

#include "Components/CapsuleComponent.h"

ACrabCharacter::ACrabCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CRAB
	(TEXT("/Game/model/animals/crab/crab.crab"));
	if (SK_CRAB.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_CRAB.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/animals/crabAnimation.crabAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	GetCapsuleComponent()->SetCapsuleRadius(91.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(91.f);
	
	/** Set the model size and adjust position */
	GetMesh()->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -12.f), FRotator(0.f, -90.f, 0.f));
}
