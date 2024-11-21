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
#include "CompileMode.h"
#include "isteamuser.h"
#include "steamclientpublic.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

UNetworkFailureManager::UNetworkFailureManager()
{
	bNextHost = false;
	DataManager = NewObject<UDataManager>(this, TEXT("DataManager"));

	static ConstructorHelpers::FClassFinder<UCapturedImageWidget> CaptureImage
	(TEXT("/Game/widget/CapturedImage.CapturedImage_C"));
	if (CaptureImage.Succeeded())
	{
		CapturedImageWidgetClass = CaptureImage.Class;
	}
}

void UNetworkFailureManager::Init()
{
#if EDITOR_MODE || LAN_MODE
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(/*STEAM_SUBSYSTEM*/);
#else
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
#endif
	this->SessionInterface = OnlineSubsystem->GetSessionInterface();

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UNetworkFailureManager::HandleNetworkFailure);
		// FWorldDelegates::LevelAddedToWorld.AddUObject(this, &UNetworkFailureManager::OnNewLevelLoaded);
		// FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UNetworkFailureManager::OnNewLevelLoaded);
		
		// Below Functions Is Not Called During Seamless Travel
		FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UNetworkFailureManager::OnNewLevelLoaded);
		// FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &UNetworkFailureManager::OnNewLevelLoaded);
	}
}

void UNetworkFailureManager::ShowCapturedTextureToPlayer(UTextureRenderTarget2D* CapturedTexture, const TArray<FColor>& Bitmap)
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

	// Texture2D�� ��ȯ
	if (CapturedTexture2D)
	{
		CapturedTexture2D->DestroyNonNativeProperties();
		CapturedTexture2D = nullptr;
	}
	CapturedTexture2D = UTexture2D::CreateTransient(CapturedTexture->SizeX, CapturedTexture->SizeY, PF_B8G8R8A8);
	if (!CapturedTexture2D)
	{
		return;
	}

	/* TEST */
	// ���������� �ʱ�ȭ�� Bitmap ������ ����
	// TArray<FColor> BlackBitmap;
	// BlackBitmap.SetNum(CapturedTexture->SizeX * CapturedTexture->SizeY);
	// for (FColor& Pixel : BlackBitmap)
	// {
	// 	Pixel = FColor::Black;  // ���������� ��� �ȼ� ����
	// }
	//
	// // �ؽ�ó ������ ����
	// void* TextureData = CapturedTexture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	// FMemory::Memcpy(TextureData, BlackBitmap.GetData(), BlackBitmap.Num() * sizeof(FColor));
	// CapturedTexture2D->GetPlatformData()->Mips[0].BulkData.Unlock();
	
	void* TextureData = CapturedTexture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, Bitmap.GetData(), Bitmap.Num() * sizeof(FColor));
	CapturedTexture2D->GetPlatformData()->Mips[0].BulkData.Unlock();
	CapturedTexture2D->UpdateResource();

	// �÷��̾� ��Ʈ�ѷ��� ���� ���� ����
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController && PlayerController->IsLocalController())
	{
		// UMG ���� ����
		CapturedImageWidget = CreateWidget<UCapturedImageWidget>(GetWorld(), CapturedImageWidgetClass);
		if (CapturedImageWidget)
		{
			// ĸó�� �ؽ�ó ����
			CapturedImageWidget->SetCapturedTexture(CapturedTexture2D);

			// ������ ȭ�鿡 �߰�
			CapturedImageWidget->AddToViewport();
		}
	}
	// FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UNetworkFailureManager::OnNewLevelLoaded);
}

void UNetworkFailureManager::DestroyPreviousSession(FOnSessionDestroyedCallback OnSessionDestroyedCallback)
{
	if (SessionInterface.IsValid())
	{
		TWeakObjectPtr<UNetworkFailureManager> WeakThis = this;
		OnDestroyCompleteDelegateHandle = SessionInterface->OnDestroySessionCompleteDelegates.AddLambda([WeakThis, OnSessionDestroyedCallback](FName SessionName, bool bWasSuccessful)
		{
			if (WeakThis.IsValid())
			{
				UNetworkFailureManager* StrongThis = WeakThis.Get();
				StrongThis->SessionInterface->OnDestroySessionCompleteDelegates.Remove(StrongThis->OnDestroyCompleteDelegateHandle);
				if (bWasSuccessful)
				{
					const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
					StrongThis->SessionNameGI = StrongThis->DesiredSessionName;
					if (GameMapsSettings)
					{
						FName DefaultLevel = FName(*GameMapsSettings->GetGameDefaultMap());
						FString DefaultLevelWithOption = DefaultLevel.ToString() + TEXT("?closed");
						FName NewLevelName = FName(*DefaultLevelWithOption);
						
						UWorld* World = StrongThis->GetWorld();
						if (World)
						{
							// (StrongThis->*OnSessionDestroyedCallback)(World);
							StrongThis->OnSessionDestroyedDelegate.Broadcast(World);
						}
						else
						{
							FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(StrongThis, &UNetworkFailureManager::OnLevelLoaded, OnSessionDestroyedCallback);
							UGameplayStatics::OpenLevel(StrongThis, NewLevelName, true);
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to destroy session: %s"), *SessionName.ToString());
				}
			}
		});

		SessionInterface->DestroySession(SessionNameGI);
	}
}

void UNetworkFailureManager::OnLevelLoaded(UWorld* LoadedWorld, FOnSessionDestroyedCallback OnSessionDestroyedCallback)
{
	// ��������Ʈ ����
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	// �Լ� ������ ȣ��
	(this->*OnSessionDestroyedCallback)(LoadedWorld);
}

void UNetworkFailureManager::CreateNewSession(UWorld* World)
{
	this->OnSessionDestroyedDelegate.Clear();
	FNetLogger::EditerLog(FColor::Green, TEXT("Creating Session..."));
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

	SessionSettings.Set(FName("BanList"), SavedBanList, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(FName("ExistingPlayersList"), ExistingPlayersList, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(SETTING_MAPNAME, DesiredMapName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(FName("SessionName"), DesiredSessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
	
	OnCreateCompleteDelegateHandle = SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkFailureManager::CreateSessionComplete, World);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	/* TODO: NAME_GameSession ��ġ�� */
	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionNameGI, SessionSettings);
}

void UNetworkFailureManager::CreateSessionComplete(FName SessionName, bool bWasSuccessful, UWorld* World)
{
	if (bWasSuccessful)
	{
		SessionInterface->OnCreateSessionCompleteDelegates.Remove(OnCreateCompleteDelegateHandle);

		FString MapName = DesiredMapName.ToString();
		FString TravelURL = FString::Printf(TEXT("%s?listen"), *MapName);
		// FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UNetworkFailureManager::OnNewLevelLoaded);
		GetWorld()->ServerTravel(TravelURL, true);
	}
	else
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Failed to create session"));
	}
}

void UNetworkFailureManager::JoinNewSession(UWorld* World)
{
	this->OnSessionDestroyedDelegate.Clear();
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	// Online Mode : False
	SessionSearch->bIsLanQuery = false;
	// Lan Mode : True
	// SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(SETTING_MAPNAME, DesiredMapName.ToString(), EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(FName("SessionName"), DesiredSessionName.ToString(), EOnlineComparisonOp::Equals);
	if (!SessionSearch->bIsLanQuery)
	{
		SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	}

	// UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(World->GetGameInstance());
	OnFindCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
	   FOnFindSessionsCompleteDelegate::CreateUObject(this, &UNetworkFailureManager::FindSessionComplete, GetWorld()));
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void UNetworkFailureManager::FindSessionComplete(bool bWasSuccessful, UWorld* World)
{
	SessionInterface->OnFindSessionsCompleteDelegates.Remove(OnFindCompleteDelegateHandle);
	if (bWasSuccessful)
	{
		bool bFoundDesiredSession = false;
		FNetLogger::LogError(TEXT("Number of Sessions Found: %d"), SessionSearch->SearchResults.Num());
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			FString SessionName;
			SearchResult.Session.SessionSettings.Get(FName("SessionName"), SessionName);
			// ���� �̸� Ȯ��
			/* TODO: �α� ����ؼ� Ȯ���غ��� */
			FNetLogger::LogError(TEXT("Session Name[JOIN]: %s"), *SessionName);
			FNetLogger::LogError(TEXT("Desired Session Name[JOIN]: %s"), *DesiredSessionName.ToString());
			if (SessionName.Contains(DesiredSessionName.ToString())) // ���ϴ� ���� �̸��� ��
			{
				// ���ϴ� ������ ã������ �� ���ǿ� ����
				JoinSession(SearchResult, World);
				bFoundDesiredSession = true;
				break;
			}
			else
			{
				FNetLogger::EditerLog(FColor::Green, TEXT("Session Name Mismatch: %s, %s"), *SessionName, *DesiredSessionName.ToString());
			}
		}
		if (!bFoundDesiredSession)
		{
			FNetLogger::EditerLog(FColor::Green, TEXT("Failed to find session: Retrying...[true]"));
			JoinNewSession(World);
		}
	}
	else
	{
		FNetLogger::EditerLog(FColor::Green,TEXT("Failed to find session: Retrying..."));
		JoinNewSession(World);
	}
}

void UNetworkFailureManager::OnNewLevelLoaded(const FWorldContext& WorldContext, const FString& LevelName)
{
	const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
	FString DefaultLevel = GameMapsSettings->GetGameDefaultMap();
	// FString CurrentLevel = World->GetMapName();
	if (bMigrating)
	{
		if (DefaultLevel.Contains(LevelName))
		{
			return ;
		}
		if (LevelName.Contains(DesiredMapName.ToString()))
		{
			bMigrating = false;
		}
	}
	else
	{
		ResetInstance();
	}
}

void UNetworkFailureManager::OnNewLevelLoaded(UWorld* World)
{
	FString LevelName = World->GetMapName();
	const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
	FString DefaultLevel = GameMapsSettings->GetGameDefaultMap();
	// FString CurrentLevel = World->GetMapName();
	if (bMigrating)
	{
		if (DefaultLevel.Contains(LevelName))
		{
			return ;
		}
		if (LevelName.Contains(DesiredMapName.ToString()))
		{
			bMigrating = false;
		}
	}
	else
	{
		ResetInstance();
	}
}

void UNetworkFailureManager::OnNewLevelLoaded(const FString& LevelName)
{
	FNetLogger::LogError(TEXT("OnNewLevelLoaded %s"), *LevelName);
	const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
	FString DefaultLevel = GameMapsSettings->GetGameDefaultMap();
	// FString CurrentLevel = World->GetMapName();
	if (bMigrating)
	{
		if (DefaultLevel.Contains(LevelName))
		{
			return ;
		}
		if (LevelName.Contains(DesiredMapName.ToString()))
		{
			bMigrating = false;
		}
	}
	else
	{
		ResetInstance();
	}
	// const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
	// if (!GameMapsSettings)
	// {
	// 	FNetLogger::EditerLog(FColor::Red, TEXT("GameMapsSettings is null"));
	// 	return;
	// }
	// FString DefaultLevel = GameMapsSettings->GetGameDefaultMap();
	// FNetLogger::EditerLog(FColor::Cyan, TEXT("Default Level: %s"), *DefaultLevel);
	// FNetLogger::EditerLog(FColor::Cyan, TEXT("Current Level: %s"), *World->GetMapName());
	// if (CapturedImageWidget && !DefaultLevel.Contains(World->GetMapName()))
	// {
	// 	FNetLogger::EditerLog(FColor::Red, TEXT("OnNewLevelLoaded"));
	// 	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	// 	CapturedImageWidget->RemoveFromParent();
	// }
	// else if (DefaultLevel.Contains(World->GetMapName()))
	// {
	// 	// UMG ���� ����
	// 	CapturedImageWidget = CreateWidget<UCapturedImageWidget>(World, CapturedImageWidgetClass);
	// 	if (CapturedImageWidget)
	// 	{
	// 		// ĸó�� �ؽ�ó ����
	// 		if (CapturedTexture2D)
	// 		{
	// 			CapturedImageWidget->SetCapturedTexture(CapturedTexture2D);
	// 			FNetLogger::EditerLog(FColor::Red, TEXT("OnNewLevelLoaded Default Level"));
	// 			CapturedImageWidget->AddToViewport(9);
	// 		}
	// 	}
	// }
}

void UNetworkFailureManager::JoinSession(const FOnlineSessionSearchResult& SearchResult, UWorld* World)
{
	FNetLogger::EditerLog(FColor::Green, TEXT("Joining Session..."));
	OnJoinCompleteDelegateHandle = SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetworkFailureManager::JoinSessionComplete, World);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), SessionNameGI, SearchResult);
}

void UNetworkFailureManager::ResetInstance()
{
	FNetLogger::EditerLog(FColor::Green, TEXT("Resetting Instance"));
	DataManager->Clear();
	DesiredMaxPlayers = 0;
	bNextHost = false;
	DesiredSessionName = NAME_None;
	DesiredMapName = NAME_None;
	if (SessionInterface.IsValid())
	{
		SessionInterface->OnDestroySessionCompleteDelegates.Remove(OnDestroyCompleteDelegateHandle);
		SessionInterface->OnCreateSessionCompleteDelegates.Remove(OnCreateCompleteDelegateHandle);
		SessionInterface->OnFindSessionsCompleteDelegates.Remove(OnFindCompleteDelegateHandle);
		SessionInterface->OnJoinSessionCompleteDelegates.Remove(OnJoinCompleteDelegateHandle);
	}
}

void UNetworkFailureManager::TryReset()
{
	if (!bMigrating)
	{
		ResetInstance();
	}
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
		JoinNewSession(GetWorld());
	}
}

void UNetworkFailureManager::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type Arg,
                                                  const FString& String)
{
	UE_LOG(LogTemp, Warning, TEXT("Network Failure: %s"), *String);
	FNetLogger::EditerLog(FColor::Red, TEXT("Network Failure: %s"), *String);

	if ((Arg == ENetworkFailure::ConnectionLost || Arg == ENetworkFailure::FailureReceived) && !bMigrating)
	{
		this->bMigrating = true;
		this->ResetInstance();
		this->SaveSessionMetaData(World);
		OnHostMigrationDelegate.Broadcast(World, DataManager);
		if (bNextHost)
		{
			OnSessionDestroyedDelegate.AddUObject(this, &UNetworkFailureManager::CreateNewSession);
			DestroyPreviousSession(&UNetworkFailureManager::CreateNewSession);
		}
		else
		{
			OnSessionDestroyedDelegate.AddUObject(this, &UNetworkFailureManager::JoinNewSession);
			DestroyPreviousSession(&UNetworkFailureManager::JoinNewSession);
		}
	}
	else
	{
		if (!bMigrating)
		{
			SessionInterface->DestroySession(SessionNameGI);
			SessionNameGI = "";
		}
	}
}

void UNetworkFailureManager::CreateNewSessionMetaData(UWorld* World, FNamedOnlineSession* CurrentSession, const FUniqueNetIdRef& NewHostPlayerID)
{
	FString PlayerName = NewHostPlayerID->ToString();

	// Create a new session name based on the player's name
	DesiredSessionName = FName(*FString::Printf(TEXT("Session_%s"), *PlayerName));
	FNetLogger::LogError(TEXT("New Session Name: %s"), *DesiredSessionName.ToString());

	DesiredMapName = FName(*FString::Printf(TEXT("%s"), *World->GetMapName()));

	SavedBanList = "";
	CurrentSession->SessionSettings.Get(FName("BanList"), SavedBanList);
	FUniqueNetIdPtr HostID = CurrentSession->OwningUserId;
	SavedBanList += TEXT(",") + HostID->ToString();

	ExistingPlayersList = "";
	CurrentSession->SessionSettings.Get(FName("ExistingPlayersList"), ExistingPlayersList);
}

void UNetworkFailureManager::CaptureViewport()
{
	// 1. Viewport ĸó
	if (GEngine && GEngine->GameViewport)
	{
		FViewport* Viewport = GEngine->GameViewport->Viewport;

		if (Viewport)
		{
			UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
			RenderTarget->InitAutoFormat(1920, 1080);

			TArray<FColor> Bitmap;
			Bitmap.SetNum(Viewport->GetSizeXY().X * Viewport->GetSizeXY().Y);

			// FIntRect Rect(0, 0, Viewport->GetSizeXY().X, Viewport->GetSizeXY().Y);

			if (Viewport->ReadPixels(Bitmap))
			{
				AsyncTask(ENamedThreads::GameThread, [this, Bitmap, RenderTarget]()
				{
					ShowCapturedTextureToPlayer(RenderTarget, Bitmap);
				});
			}
		}
	}
}

void UNetworkFailureManager::SaveSessionMetaData(UWorld* World)
{
	FNetLogger::LogError(TEXT("Name of Session: %s"), *SessionNameGI.ToString());
	FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(SessionNameGI);
	if (CurrentSession)
	{
		TArray<FUniqueNetIdRef> RegisteredPlayers;
		TSet<FString> UniqueNicknames;  // �ߺ� Ȯ���� ���� �г��� �����

		FNetLogger::LogError(TEXT("Num of Registered Players: %d"), CurrentSession->RegisteredPlayers.Num());
		for (int32 i = 0; i < CurrentSession->RegisteredPlayers.Num(); i++)
		{
			FUniqueNetIdRef PlayerID = CurrentSession->RegisteredPlayers[i];
			/* NOTE: Steam�� IdentityInterface�� ����� ��� GetPlayerNickName�� ����� �� ����.
			* OnlineIdentityInterfaceSteam.cpp ���� �ڵ带 Ȯ���غ��� ���ʿ� �Ѱ��� Parameter�� �˻��� ���� �ʴ� ���� �� �� �ִ�. ��, ������ �Լ� ��ü�� �߸���
			* FString PlayerNickname = IdentityInterface->GetPlayerNickname(*PlayerID);
			* ��, �ڱ� �ڽ��� �̸��� �������� �� ���̴�.
			*/

			// ù ��° �÷��̾�(Original Host)�� �������� ����
			FUniqueNetIdPtr HostID = CurrentSession->OwningUserId;
			if (HostID->ToString().Contains(PlayerID->ToString()))
			{
				continue;
			}

			// �ߺ��� �г����� �ִ��� Ȯ��
			if (UniqueNicknames.Contains(PlayerID->ToString()))
			{
				continue;  // �ߺ��� �г����� �߰����� ����
			}
			// Not Include the First Player(Original Host)
			RegisteredPlayers.Add(CurrentSession->RegisteredPlayers[i]);
			UniqueNicknames.Add(PlayerID->ToString());
			DesiredMaxPlayers += 1;
		}
		auto CompareByNickname = [&World](const FUniqueNetIdRef& A, const FUniqueNetIdRef& B) -> bool
		{
			IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);
    
			if (!A->IsValid() || !B->IsValid() || !IdentityInterface.IsValid())
			{
				return false;
			}
    
			FString NicknameA = A->ToString();
			FString NicknameB = B->ToString();

			return NicknameA > NicknameB;  // ���ڿ��� �������� ������ ����
		};
		
		RegisteredPlayers.Sort(CompareByNickname);
		
		for (int32 i = 0; i < RegisteredPlayers.Num(); i++)
		{
			FUniqueNetIdRef PlayerID = RegisteredPlayers[i];
			FString PlayerNickname = PlayerID->ToString();
		}
		
		const FUniqueNetIdRepl LocalPlayerID = World->GetFirstLocalPlayerFromController()->GetUniqueNetIdFromCachedControllerId();
		const FString LocalPlayerNickname = LocalPlayerID->ToString();
		const FUniqueNetIdRef NextHostID = RegisteredPlayers[0];
		const FString NextHostNickname = NextHostID->ToString();
		
		if (NextHostID->IsValid() && NextHostNickname.Contains(LocalPlayerNickname))
		{
			bNextHost = true;
		}
		else
		{
			bNextHost = false;
		}
		CreateNewSessionMetaData(World, CurrentSession, NextHostID);
		// CaptureViewport();
	}
	else
	{
		FNetLogger::LogError(TEXT("CurrentSession is null"));
	}
}

bool UNetworkFailureManager::ValidateAddr(FString& Addr)
{
	FString Address;
	FString PortStr;
	if (Addr.Split(":", &Address, &PortStr))
	{
		// ��Ʈ�� int32�� ��ȯ
		int32 Port = FCString::Atoi(*PortStr);

		// ��Ʈ�� 0���� Ȯ��
		if (Port == 0)
		{
			FNetLogger::LogError(TEXT("Port is 0, leaving session and retrying"));
			return false; // ��Ʈ�� 0�̸� �� �̻� �������� ����
		}
	}
	return true;
}
