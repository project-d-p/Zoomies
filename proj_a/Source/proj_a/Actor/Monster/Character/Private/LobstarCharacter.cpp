#include "LobstarCharacter.h"

#include "Components/CapsuleComponent.h"

ALobstarCharacter::ALobstarCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_MAMMOTH
	(TEXT("/Game/model/animals/lobstar/Object_7.Object_7"));
	if (SK_MAMMOTH.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_MAMMOTH.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/animals/lobstarAnimation.lobstarAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}

	/** Set the Capsule size */
	GetCapsuleComponent()->SetCapsuleRadius(65.f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(65.f);
	
	/** Set the model size and adjust position */
	GetMesh()->SetRelativeScale3D(FVector(3.f, 3.f, 3.f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -20.f), FRotator(0.f, -90.f, 0.f));
}