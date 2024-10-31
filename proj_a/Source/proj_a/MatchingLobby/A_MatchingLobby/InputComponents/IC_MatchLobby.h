#pragma once

#include "CoreMinimal.h"
#include "BaseInputComponent.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"
#include "IC_MatchLobby.generated.h"

class ULC_MatchLobby;

UCLASS()
class UIC_MatchLobby : public UActorComponent
{
	GENERATED_BODY()
public:
	UIC_MatchLobby();
	
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;
	
	void Set_PC(APC_MatchingLobby* Apc_MatchingLobby);
	void Set_CHAR(APawn* Pawn);
	void Set_LC(ULC_MatchLobby* Lc_MatchLobby);
	
private:
	void BindMatchLobbyActions();
	void UnbindMatchLobbyActions();

	void Move(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);
	void Rotate(const FInputActionValue& value);
	void Interact(const FInputActionValue& value);
	void ESC(const FInputActionValue& value);

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* MoveAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* JumpAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* RotateAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* InteractAction;
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ESCAction;
	
protected:
	APC_MatchingLobby* Get_PC() const;
	APawn* Get_CHAR();
	
	APC_MatchingLobby* PC_MatchLobby = nullptr;
	APawn* CHAR_MatchLobby = nullptr;
	
	UPROPERTY()
	class ULC_MatchLobby* OwningLevelComponent = nullptr;
};
