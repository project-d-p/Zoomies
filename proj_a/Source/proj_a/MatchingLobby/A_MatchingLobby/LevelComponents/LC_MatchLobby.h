#pragma once

#include "CoreMinimal.h"
#include "BaseLevelComponent.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"
#include "../InputComponents/IC_MatchLobby.h"
#include "LC_MatchLobby.generated.h"

class UIC_MatchLobby;

UCLASS()
class ULC_MatchLobby : public UActorComponent
{
	GENERATED_BODY()
	
	public:
	ULC_MatchLobby();

	UFUNCTION(BlueprintCallable)
	void SetFixedCameraView();
	
	/* Activate and Deactivate this Component */
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	
	protected:
		UPROPERTY()
		UIC_MatchLobby* InputComponent;
		UPROPERTY()
		UChatManager* ChatManager;
		UPROPERTY()
		USoundComponent* SoundComponent;
	
		APC_MatchingLobby* PC_MatchLobby;
		APawn* CHAR_MatchLobby;
	
		APC_MatchingLobby* GetPlayerController() const;
		APawn* GetPlayerCharacter();
		
	public:
		
		void Set_PC(APC_MatchingLobby* AdpPlayerController);
		void Set_CHAR(APawn* InCharacter);
		
		UIC_MatchLobby* GetInputComponent() const;
		USoundComponent* GetSoundComponent() const;
};
