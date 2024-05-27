#include "MyUserWidget.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Components/Button.h"

void UMyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Player1_testButten)
	{
		Player1_testButten->OnClicked.AddDynamic(this, &UMyUserWidget::OnPlayer1_testButtenClicked);
	}
	if (Player2_testButten)
	{
		Player2_testButten->OnClicked.AddDynamic(this, &UMyUserWidget::OnPlayer2_testButtenClicked);
	}
}

bool UMyUserWidget::JoinP2PSession(FName SessionName)
{
	// ���� �������̽� ��ȿ�� �˻�
	if (!Sessions.IsValid())
	{	
		// log
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Game Session Interface is invailed")));
		}
	}
 
	// Find Session Complete Delegate ���
	Sessions->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMyUserWidget::OnFindSessionsComplete)
	);
 
	// Find Game Session
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 42;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	// �ǹ̴� ������, �˻� ���ǿ� ���� ���÷μ� ����. ���� ����.
	SessionSearch->QuerySettings.Set(FName("MaxPlayers"), 2, EOnlineComparisonOp::Equals);
 
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	Sessions->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
	return true;
}


void UMyUserWidget::OnFindSessionsComplete(bool bWasSuccessful)
{
	if(!Sessions.IsValid() || !bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find sessions."));
		return;	
	}
 
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Cyan,
			FString(TEXT("======== Search Result ========")));
	}
 
	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;
		
		// ��ġ Ÿ�� Ȯ���ϱ�
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);
		
		// ã�� ������ ���� ����ϱ�
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Session ID : %s / Owner : %s"), *Id, *User));
		}
 
		// ������ ��ġ Ÿ���� "FreeForAll"�� ��� ���� ����
		if (MatchType == FString("FreeForAll"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Cyan,
					FString::Printf(TEXT("Joining Match Type : %s"), *MatchType));
			}
 
			// Join Session Complete Delegate ��� 
			Sessions->AddOnJoinSessionCompleteDelegate_Handle(
				FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMyUserWidget::OnJoinSessionComplate)	
			);
 
			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			Sessions->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		}
	}
}

void UMyUserWidget::OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!Sessions.IsValid())
		return;
 
	// ���ǿ� �����ߴٸ� IP Address���ͼ� �ش� ������ ����
	FString Address;
	if (Sessions->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString::Printf(TEXT("Connect String : %s"), *Address));
		}
 
		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UMyUserWidget::OnCreateSessionComplete(FName SessionName, bool bWasSuccssful)
{
	if (bWasSuccssful)
	{
		GetWorld()->ServerTravel("/Game/level/mainLevel?listen");
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Created session : %s"), *SessionName.ToString()));
		}
	} 
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session!")));
		}
	}
}

bool UMyUserWidget::CreateP2PSession(FName SessionName, int32 MaxPlayers)
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = MaxPlayers;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Sessions->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMyUserWidget::OnCreateSessionComplete)
	);
	
	return Sessions->CreateSession(0, SessionName, SessionSettings);
}

void UMyUserWidget::InitializeSteamAPI()
{
	// Retrieve the Steam subsystem.
	IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get();
	if (!SteamSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve the Steam subsystem."));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Steam subsystem retrieved."));

	// Access the OnlineSubsystem
	// Get the online session interface
	Sessions = SteamSubsystem->GetSessionInterface();
	if (!Sessions.IsValid())
		UE_LOG(LogTemp, Error, TEXT("Session interface is not valid."));
	if (!Sessions) {
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve the session interface."));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Session interface retrieved."));
	
	if (GEngine)
	{
		// OnlineSubsystem �̸� ����ϱ�
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Blue,
			FString::Printf(TEXT("Subsystem %s"), *SteamSubsystem->GetSubsystemName().ToString()));
	}
}

void UMyUserWidget::OnPlayer1_testButtenClicked()
{
	// ���⿡ ��ư Ŭ�� �� ������ �ڵ带 �ۼ��մϴ�.
	UE_LOG(LogTemp, Warning, TEXT("Player1_testButten was clicked!"));
	InitializeSteamAPI();
	CreateP2PSession("TestSession", 3);
}

void UMyUserWidget::OnPlayer2_testButtenClicked()
{
	// ���⿡ ��ư Ŭ�� �� ������ �ڵ带 �ۼ��մϴ�.
	UE_LOG(LogTemp, Warning, TEXT("Player2_testButten was clicked!"));
	InitializeSteamAPI();
	JoinP2PSession("TestSession");
}