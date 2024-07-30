#include "SteamSocketP2P.h"

#include "FNetLogger.h"
#include "isteamnetworkingutils.h"
#include "OnlineSessionSettings.h"
#include "SteamSocketIP.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

UISocketInterface* USteamSocketP2P::Clone() const
{
	return NewObject<USteamSocketIP>();
}

void USteamSocketP2P::ActivateServer()
{
	Super::ActivateServer();
	this->CheckRelayNetworkStatus();

	int nLocalVirtualPort = 0;
	m_ListenSocket = SteamNetworkingSockets()->CreateListenSocketP2P(nLocalVirtualPort, 0, nullptr);
	
	if (m_ListenSocket == k_HSteamListenSocket_Invalid)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create listen socket"));
		check(false)
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Listen socket created successfully"));
	}
}

void USteamSocketP2P::ActivateClient()
{
	Super::ActivateClient();
	this->CheckRelayNetworkStatus();

	SteamNetworkingIdentity identityRemote;
	identityRemote.SetSteamID(this->GetHostSteamID());


	int nRemoteVirtualPort = 0; // 상대방의 가상 포트 번호와 일치해야 함
	m_Connection = SteamNetworkingSockets()->ConnectP2P(identityRemote, nRemoteVirtualPort, 0, nullptr);

	if (m_Connection == k_HSteamNetConnection_Invalid)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to connect to server"));
		check(false);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Connected to server"));
		AddConnection(m_Connection);
	}
}

USteamSocketP2P::~USteamSocketP2P()
{
	
}

CSteamID USteamSocketP2P::GetHostSteamID()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	check(OnlineSub);

	IOnlineSessionPtr SessionPtr = OnlineSub->GetSessionInterface();
	check(SessionPtr);

	FNamedOnlineSession* Session = SessionPtr->GetNamedSession(NAME_GameSession);
	check(Session);

	TSharedPtr<const FUniqueNetId> hostNetID = Session->OwningUserId;
	check(hostNetID.IsValid());

	// Change UniqueNetId to FString
	FString HostSteamIDString = hostNetID->ToString();
                    
	// Change FString to uint64
	uint64 HostSteamID64 = FCString::Atoi64(*HostSteamIDString);
                    
	// Create CSteamID
	CSteamID hostSteamID = CSteamID(HostSteamID64);

	return hostSteamID;
}

void USteamSocketP2P::CheckRelayNetworkStatus()
{
	while (true)
	{
		SteamRelayNetworkStatus_t status;
		ESteamNetworkingAvailability availability = SteamNetworkingUtils()->GetRelayNetworkStatus(&status);

		if (availability == k_ESteamNetworkingAvailability_Current || availability == k_ESteamNetworkingAvailability_Unknown)
		{
			FNetLogger::LogError(TEXT("SDR is now available"));
			break ;
		}
		else if (availability == k_ESteamNetworkingAvailability_Attempting)
		{
			FNetLogger::LogError(TEXT("SDR is attempting to be available"));
		}
		else
		{
			FNetLogger::LogError(TEXT("SDR is not available"));
		}
	}
}
