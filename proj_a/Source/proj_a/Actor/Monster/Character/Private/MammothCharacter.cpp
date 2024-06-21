#include "MammothCharacter.h"

#include "FNetLogger.h"
#include "Components/CapsuleComponent.h"

AMammothCharacter::AMammothCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MAMMOTH(TEXT("/Game/model/animals/mammoth/sm_mammoth.sm_mammoth"));
	if (SK_MAMMOTH.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_MAMMOTH.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/animals/mammothAnimation.mammothAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	GetCapsuleComponent()->SetCapsuleRadius(60.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(180.f);
	
	/** Set the model size and adjust position */
	GetMesh()->SetRelativeScale3D(FVector(3.00f, 3.00f, 3.00f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -172.f), FRotator(0.f, -90.f, 0.f));
}