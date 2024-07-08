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
	GetMesh()->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -87.f), FRotator(0.f, 90.f, 0.f));
}
