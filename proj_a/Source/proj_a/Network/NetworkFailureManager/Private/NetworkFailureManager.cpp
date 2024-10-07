#include "NetworkFailureManager.h"

#include "FNetLogger.h"
#include "GameMapsSettings.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
// #include "Engine/GameMapsSettings.h"
#include "CapturedImageWidget.h"
#include "Engine/GameViewportClient.h"
#include "Algo/Sort.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

UNetworkFailureManager::UNetworkFailureManager()
{
	bNextHost = false;
	DataManager = NewObject<UDataManager>();
}

void UNetworkFailureManager::Init()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(/*STEAM_SUBSYSTEM*/);
	this->SessionInterface = OnlineSubsystem->GetSessionInterface();

	if (GEngine)
	{
		FNetLogger::EditerLog(FColor::Green, TEXT("NetworkFailureManager::Init"));
		GEngine->OnNetworkFailure().AddUObject(this, &UNetworkFailureManager::HandleNetworkFailure);
	}
}

void UNetworkFailureManager::ShowCapturedTextureToPlayer(UTextureRenderTarget2D* CapturedTexture,
	const TArray<FColor>& Bitmap)
{
	if (!CapturedTexture)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Texture2D로 변환
	UTexture2D* CapturedTexture2D = UTexture2D::CreateTransient(CapturedTexture->SizeX, CapturedTexture->SizeY, PF_B8G8R8A8);
	if (!CapturedTexture2D)
	{
		return;
	}

	void* TextureData = CapturedTexture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, Bitmap.GetData(), Bitmap.Num() * sizeof(FColor));
	CapturedTexture2D->GetPlatformData()->Mips[0].BulkData.Unlock();

	// 플레이어 컨트롤러를 통해 위젯 생성
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController && PlayerController->IsLocalController())
	{
		// UMG 위젯 생성
		CapturedImageWidget = CreateWidget<UCapturedImageWidget>(PlayerController, UCapturedImageWidget::StaticClass());
		if (CapturedImageWidget)
		{
			// 캡처된 텍스처 설정
			CapturedImageWidget->SetCapturedTexture(CapturedTexture2D);

			// 위젯을 화면에 추가
			CapturedImageWidget->AddToViewport();
		}
	}
}

void UNetworkFailureManager::DestroyPreviousSession(FOnSessionDestroyedCallback OnSessionDestroyedCallback)
{
	if (SessionInterface.IsValid())
	{
		OnDestroyCompleteDelegateHandle = SessionInterface->OnDestroySessionCompleteDelegates.AddLambda([this, OnSessionDestroyedCallback](FName SessionName, bool bWasSuccessful)
		{
			this->SessionInterface->OnDestroySessionCompleteDelegates.Remove(this->OnDestroyCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
				if (GameMapsSettings)
				{
					
					FName DefaultLevel = FName(*GameMapsSettings->GetGameDefaultMap());
					FNetLogger::EditerLog(FColor::Green, TEXT("Default Level: %s"), *DefaultLevel.ToString());
					FNetLogger::LogError(TEXT("Default Level: %s"), *DefaultLevel.ToString());
				
					FString DefaultLevelWithOption = DefaultLevel.ToString() + TEXT("?closed");
					FName NewLevelName = FName(*DefaultLevelWithOption);
				
					UWorld* World = this->GetOuter()->GetWorld();
					if (World)
					{
						FNetLogger::EditerLog(FColor::Cyan, TEXT("Destroy Session Complete CallBack"));
						(this->*OnSessionDestroyedCallback)(World);
					}
					else
					{
						FNetLogger::EditerLog(FColor::Cyan, TEXT("Cannt Get World"));
						FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UNetworkFailureManager::OnLevelLoaded, OnSessionDestroyedCallback);
						UGameplayStatics::OpenLevel(this, NewLevelName, true);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to destroy session: %s"), *SessionName.ToString());
			}
		});

		SessionInterface->DestroySession(NAME_GameSession);
	}
}

void UNetworkFailureManager::OnLevelLoaded(UWorld* LoadedWorld, FOnSessionDestroyedCallback OnSessionDestroyedCallback)
{
	// 레벨이 완전히 로드된 후 호출
	FNetLogger::EditerLog(FColor::Green, TEXT("Level Loaded Successfully"));
	
	// 델리게이트 해제
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	// 함수 포인터 호출
	(this->*OnSessionDestroyedCallback)(LoadedWorld);
}

void UNetworkFailureManager::CreateNewSession(UWorld* World)
{
	// Online Mode : False
	SessionSettings.bIsLANMatch = false;
	// Lan Mode : True
	// SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 4; // Number of players
	SessionSettings.bShouldAdvertise = true; // Advertise the session to others
	SessionSettings.bAllowJoinInProgress = true; // Allow joining in progress
	SessionSettings.bAllowJoinViaPresence = true; // Allow joining via presence (show sessions to players in current regions)
	SessionSettings.bUsesPresence = true; // Use presence for the session
	SessionSettings.bAllowInvites = true;
	if (!SessionSettings.bIsLANMatch)
	{
		SessionSettings.bUseLobbiesIfAvailable = true;
	}

	SessionSettings.Set(SETTING_MAPNAME, DesiredMapName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(FName("SESSION_NAME"), DesiredSessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
	
	OnCreateCompleteDelegateHandle = SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkFailureManager::CreateSessionComplete, World);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings);
}

void UNetworkFailureManager::CreateSessionComplete(FName SessionName, bool bWasSuccessful, UWorld* World)
{
	if (bWasSuccessful)
	{
		FNetLogger::EditerLog(FColor::Green, TEXT("Session Created Successfully"));
		SessionInterface->OnCreateSessionCompleteDelegates.Remove(OnCreateCompleteDelegateHandle);

		FString MapName = DesiredMapName.ToString();
		FString TravelURL = FString::Printf(TEXT("%s?listen"), *MapName);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UNetworkFailureManager::OnNewLevelLoaded);
		GetWorld()->ServerTravel(TravelURL, true);
	}
	else
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Failed to create session"));
	}
}

void UNetworkFailureManager::JoinNewSession(UWorld* World)
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	// Online Mode : False
	SessionSearch->bIsLanQuery = false;
	// Lan Mode : True
	// SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(SETTING_MAPNAME, DesiredMapName.ToString(), EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(FName("SESSION_NAME"), DesiredSessionName.ToString(), EOnlineComparisonOp::Equals);
	if (!SessionSearch->bIsLanQuery)
	{
		SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	}

	OnFindCompleteDelegateHandle = SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetworkFailureManager::FindSessionComplete, World);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void UNetworkFailureManager::FindSessionComplete(bool bWasSuccessful, UWorld* World)
{
	SessionInterface->OnFindSessionsCompleteDelegates.Remove(OnFindCompleteDelegateHandle);
	if (bWasSuccessful)
	{
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			FString SessionName;
			SearchResult.Session.SessionSettings.Get(FName("SESSION_NAME"), SessionName);
			// 세션 이름 확인
			if (SessionName == DesiredSessionName) // 원하는 세션 이름과 비교
			{
				// 원하는 세션을 찾았으면 그 세션에 접속
				JoinSession(SearchResult, World);
				break;
			}
		}
	}
	else
	{
		FNetLogger::EditerLog(FColor::Green,TEXT("Failed to find session: Retrying..."));
		JoinNewSession(World);
	}
}

void UNetworkFailureManager::OnNewLevelLoaded(UWorld* World)
{
	if (CapturedImageWidget)
	{
		CapturedImageWidget->RemoveFromParent();
	}
}

void UNetworkFailureManager::JoinSession(const FOnlineSessionSearchResult& SearchResult, UWorld* World)
{
	FNetLogger::EditerLog(FColor::Green, TEXT("Joining Session..."));
	OnJoinCompleteDelegateHandle = SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetworkFailureManager::JoinSessionComplete, World);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult);
}

void UNetworkFailureManager::JoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result, UWorld* World)
{
	SessionInterface->OnJoinSessionCompleteDelegates.Remove(OnJoinCompleteDelegateHandle);
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FNetLogger::EditerLog(FColor::Green, TEXT("Joined Session Successfully"));
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			FString ConnectString;
			if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
			{
				if (!ValidateAddr(ConnectString))
				{
					FNetLogger::EditerLog(FColor::Green, TEXT("Address Invalid, Trying Again .."));
					SessionInterface->DestroySession(SessionName);
					JoinNewSession(World);
				}
				FString MapName;
				FOnlineSessionSettings* SessionSettingBySearch = SessionInterface->GetSessionSettings(SessionName);
				if (SessionSettingBySearch && SessionSettingBySearch->Get(SETTING_MAPNAME, MapName))
				{
					FString URL = FString::Printf(TEXT("%s/%s"), *ConnectString, *MapName);
					FNetLogger::EditerLog(FColor::Green, TEXT("ClientTravel URL: %s"), *URL);
					FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UNetworkFailureManager::OnNewLevelLoaded);
					PlayerController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
				}
				else
				{
					FNetLogger::LogError(TEXT("Failed to get map name from session settings"));
				}
			}
			else
			{
				FNetLogger::LogError(TEXT("Failed to get resolved connect string"));
			}
		}
		else
		{
			FNetLogger::LogError(TEXT("PlayerController is null"));
		}
	}
	else
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Failed to join session"));
	}
}

void UNetworkFailureManager::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg,
                                                  const FString& String)
{
	UE_LOG(LogTemp, Warning, TEXT("Network Failure: %s"), *String);
	FNetLogger::EditerLog(FColor::Red, TEXT("Network Failure: %s"), *String);

	if (Arg == ENetworkFailure::ConnectionLost)
	{
		this->SaveSessionMetaData(World);
		OnHostMigrationDelegate.Broadcast(World, DataManager);
		if (bNextHost)
		{
			// OnHostMigrationDelegate.Broadcast(World, DataManager);
			DestroyPreviousSession(&UNetworkFailureManager::CreateNewSession);
		}
		else
		{
			DestroyPreviousSession(&UNetworkFailureManager::JoinNewSession);
		}
	}
}

void UNetworkFailureManager::CreateNewSessionMetaData(UWorld* World, const FUniqueNetIdRepl& NewHostPlayerID)
{
	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);
	if (IdentityInterface.IsValid())
	{
		FString PlayerName = IdentityInterface->GetPlayerNickname(*NewHostPlayerID);

		// Create a new session name based on the player's name
		DesiredSessionName = FName(*FString::Printf(TEXT("Session_%s"), *PlayerName));
		FNetLogger::EditerLog(FColor::Red, TEXT("New Session Name: %s"), *DesiredSessionName.ToString());

		DesiredMapName = FName(*FString::Printf(TEXT("%s"), *World->GetMapName()));
		FNetLogger::EditerLog(FColor::Red, TEXT("New Map Name: %s"), *DesiredMapName.ToString());
	}
}

void UNetworkFailureManager::CaptureViewport()
{
	// 1. Viewport 캡처
	if (GEngine && GEngine->GameViewport)
	{
		FViewport* Viewport = GEngine->GameViewport->Viewport;

		if (Viewport)
		{
			// Render Target 생성
			UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
			RenderTarget->InitAutoFormat(1920, 1080);

			// 캡처된 화면을 저장하기 위해 버퍼를 생성
			TArray<FColor> Bitmap;

			// 렌더 타겟의 크기 설정
			FIntRect Rect(0, 0, Viewport->GetSizeXY().X, Viewport->GetSizeXY().Y);

			// Viewport에서 픽셀 읽기
			if (Viewport->ReadPixels(Bitmap, FReadSurfaceDataFlags(), Rect))
			{
				// 비동기로 렌더링 리소스를 처리하여 텍스처로 변환
				AsyncTask(ENamedThreads::GameThread, [this, Bitmap, RenderTarget]()
				{
					// Bitmap 데이터를 사용해 텍스처를 업데이트하고, 플레이어에게 보여줄 수 있도록 설정
					ShowCapturedTextureToPlayer(RenderTarget, Bitmap);
				});
			}
		}
	}
}

void UNetworkFailureManager::SaveSessionMetaData(UWorld* World)
{
	FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (CurrentSession)
	{
		TArray<FUniqueNetIdRepl> RegisteredPlayers;
		TSet<FString> UniqueNicknames;  // 중복 확인을 위한 닉네임 저장소
		
		for (int32 i = 0; i < CurrentSession->RegisteredPlayers.Num(); i++)
		{
			IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);
			FString PlayerNickname = IdentityInterface->GetPlayerNickname(*CurrentSession->RegisteredPlayers[i]);

			// 첫 번째 플레이어(Original Host)는 포함하지 않음
			if (PlayerNickname == CurrentSession->OwningUserId->ToString())
			{
				continue;
			}

			// 중복된 닉네임이 있는지 확인
			if (UniqueNicknames.Contains(PlayerNickname))
			{
				FNetLogger::EditerLog(FColor::Yellow, TEXT("Duplicate nickname: %s, skipping."), *PlayerNickname);
				continue;  // 중복된 닉네임은 추가하지 않음
			}
			// Not Include the First Player(Original Host)
			RegisteredPlayers.Add(CurrentSession->RegisteredPlayers[i]);
			UniqueNicknames.Add(PlayerNickname);
			DesiredMaxPlayers += 1;
			FNetLogger::EditerLog(FColor::Blue, TEXT("Player %d: %s"), i, *(IdentityInterface->GetPlayerNickname(*CurrentSession->RegisteredPlayers[i])));
		}
		auto CompareByNickname = [&World](const FUniqueNetIdRepl& A, const FUniqueNetIdRepl& B) -> bool
		{
			IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);
    
			if (!A.IsValid() || !B.IsValid() || !IdentityInterface.IsValid())
			{
				return false;
			}
    
			FString NicknameA = IdentityInterface->GetPlayerNickname(*A.GetUniqueNetId());
			FString NicknameB = IdentityInterface->GetPlayerNickname(*B.GetUniqueNetId());

			return NicknameA < NicknameB;  // 문자열을 기준으로 사전순 정렬
		};
		
		RegisteredPlayers.Sort(CompareByNickname);
		for (int32 i = 0; i < RegisteredPlayers.Num(); i++) {
			IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);
			FNetLogger::EditerLog(FColor::Cyan, TEXT("Player %d: %s"), i, *(IdentityInterface->GetPlayerNickname(*RegisteredPlayers[i])));
		}
		const FUniqueNetIdRepl LocalPlayerID = World->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId();
		const FUniqueNetIdRepl NextHostID = RegisteredPlayers[0];
		if (NextHostID.IsValid() && NextHostID == LocalPlayerID)
		{
			bNextHost = true;
		}
		else
		{
			bNextHost = false;
		}
		CreateNewSessionMetaData(World, NextHostID);
		CaptureViewport();
	}
	else
	{
		FNetLogger::EditerLog(FColor::Green, TEXT("CurrentSession is null"));
	}
}

bool UNetworkFailureManager::ValidateAddr(FString& Addr)
{
	FString Address;
	FString PortStr;
	if (Addr.Split(":", &Address, &PortStr))
	{
		// 포트를 int32로 변환
		int32 Port = FCString::Atoi(*PortStr);

		// 포트가 0인지 확인
		if (Port == 0)
		{
			FNetLogger::LogError(TEXT("Port is 0, leaving session and retrying"));
			return false; // 포트가 0이면 더 이상 진행하지 않음
		}
	}
	return true;
}
