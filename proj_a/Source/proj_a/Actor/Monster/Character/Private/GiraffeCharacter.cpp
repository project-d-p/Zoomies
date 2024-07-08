#include "GiraffeCharacter.h"

#include "Components/CapsuleComponent.h"


AGiraffeCharacter::AGiraffeCharacter()
{
	/** Loading models */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_GIRAFFE
	(TEXT("/Game/model/animals/giraffe/giraffe-lowpoly.giraffe-lowpoly"));
	if (SK_GIRAFFE.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SK_GIRAFFE.Object);
	}

	/** Loading animations */
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/animals/giraffeAnimation.giraffeAnimation_C"));
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
