#pragma once

#include "CoreMinimal.h"
#include "actor_position.pb.h"
#include "DPPlayerState.h"
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
	void SyncMyself(ADPCharacter* AdpCharacter);

private:
	void SetState(ADPCharacter* character);
	void SyncPosition(ADPCharacter* character);
	void SyncOrientation(ADPCharacter* character);
	void SyncJump(ADPCharacter* character);

	
	ADPPlayerState* player_state_ = nullptr;
	ActorPosition actor_position_;
	Jump jump_;
};
