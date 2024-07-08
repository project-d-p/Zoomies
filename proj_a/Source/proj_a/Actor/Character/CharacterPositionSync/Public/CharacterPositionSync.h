#pragma once

#include "CoreMinimal.h"
#include "actor_position.pb.h"
#include "DPPlayerState.h"
#include "gunfire.pb.h"
#include "jump.pb.h"
#include "CharacterPositionSync.generated.h"
/**
 * 
 */

class ADPCharacter;

UCLASS()
class UCharacterPositionSync : public UActorComponent
{
	GENERATED_BODY()
public:
	UCharacterPositionSync();
	virtual ~UCharacterPositionSync() override;
	void SyncWithServer(ADPCharacter* character);
	void SyncMyself(ADPCharacter* character);
	void SyncGunFire(ADPCharacter* character);
	void SyncCatch(ADPCharacter* character);
	void SyncReturnAnimal(ADPCharacter* character);

private:
	void PlayAimAnimation(ADPCharacter* character);
	void SetState(ADPCharacter* character);
	void SyncPosition(ADPCharacter* character);
	void SyncOrientationWithRotation(ADPCharacter* Character);
	void SyncOrientationWithController(ADPCharacter* Character);
	void SyncOrientation(ADPCharacter* character);
	void SyncJump(ADPCharacter* character);

	UPROPERTY()
	ADPPlayerState* Player_State = nullptr;
	
	ActorPosition actor_position_;
	Gunfire gunfire_;
	Jump jump_;
};
