#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "ChatUI.h"
#include "MyUserWidget.generated.h"

UCLASS()
class PROJ_A_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPlayer1_testButtenClicked();
	UFUNCTION()
	void OnPlayer2_testButtenClicked();
	UFUNCTION()
	void OnScoreBT1Clicked();
	UFUNCTION()
	void OnScoreBT2Clicked();
	
	UFUNCTION()
	void InitializeSteamAPI();
	UFUNCTION()
	bool CreateP2PSession(FName SessionName, int32 MaxPlayers);
	UFUNCTION()
	bool JoinP2PSession(FName SessionName);
	UFUNCTION()
	void OnFindSessionsComplete(bool bWasSuccessful);
	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccssful);
	void OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
protected:
	// XXX: 나중에 사용하도록 변경
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Player1_testButten;
	UPROPERTY(meta = (BindWidget))
	class UButton* Player2_testButten;
	UPROPERTY(meta = (BindWidget))
	class UButton* ScoreBT1;
	UPROPERTY(meta = (BindWidget))
	class UButton* ScoreBT2;

	IOnlineSessionPtr Sessions;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	// UPROPERTY(meta = (BindWidget))
	// UEditableTextBox* ChatBox;
	// UChatUI* ChatUI = nullptr;
};
