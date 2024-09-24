#include "EngineSocket.h"

#include "FNetLogger.h"
#include "Marshaller.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

UEngineSocket::UEngineSocket()
	: ListenSocket(nullptr)
	, ClientSocket(nullptr)
	, MaxClients(0)
	, bIsServer(false)
{
}

UEngineSocket::~UEngineSocket()
{
	// 모든 연결된 소켓 닫기
	for (FSocket* Socket : Connections)
	{
		if (Socket)
		{
			Socket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		}
	}

	// 리슨 소켓 닫기
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}

	// 클라이언트 소켓 닫기
	if (ClientSocket)
	{
		ClientSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
	}
}

UISocketInterface* UEngineSocket::Clone() const
{
	return NewObject<UEngineSocket>();
}

void UEngineSocket::ActivateServer()
{
	Super::ActivateServer();

	SetAsServer();
	
	FNetLogger::LogError(TEXT("Server Activated"));
	
	// 서버 주소와 포트 설정
	FString Address = TEXT("0.0.0.0"); // 모든 IP에서 수신
	int32 Port = SERVER_PORT; // 원하는 포트 번호로 설정

	// 소켓 서브시스템 가져오기
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	// IP 주소 생성
	TSharedRef<FInternetAddr> LocalAddr = SocketSubsystem->CreateInternetAddr();
	bool bIsValid;
	LocalAddr->SetIp(*Address, bIsValid);
	LocalAddr->SetPort(Port);

	if (!bIsValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid IP address."));
		check(false);
		return;
	}

	// 소켓 생성 (TCP 소켓 사용)
	ListenSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("ListenSocket"), false);

	if (!ListenSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create socket."));
		check(false);
		return;
	}

	// 소켓 옵션 설정 (재사용 및 논블로킹 모드)
	ListenSocket->SetReuseAddr(true);
	ListenSocket->SetNonBlocking(true);

	// 소켓 바인딩
	if (!ListenSocket->Bind(*LocalAddr))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to bind socket."));
		CloseConnection(ListenSocket);
		check(false);
		return;
	}

	// 리스닝 시작 (대기열의 최대 연결 수는 16으로 설정)
	if (!ListenSocket->Listen(16))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to listen on socket."));
		CloseConnection(ListenSocket);
		check(false);
		return;
	}

	FNetLogger::LogError(TEXT("Listening on port %d"), Port);
}

void UEngineSocket::ActivateClient()
{
	Super::ActivateClient();

	SetAsClient();

	FNetLogger::LogError(TEXT("Client Activated"));
	
	// 서버 주소와 포트 설정
	FString ServerAddress = "127.0.0.1"; // 서버 IP 주소로 설정
	int32 ServerPort = SERVER_PORT; // 원하는 포트 번호로 설정

	// 소켓 서브시스템 가져오기
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	// 서버 주소 생성
	TSharedRef<FInternetAddr> RemoteAddr = SocketSubsystem->CreateInternetAddr();
	bool bIsValid = false;
	RemoteAddr->SetIp(*ServerAddress, bIsValid);
	RemoteAddr->SetPort(ServerPort);

	if (!bIsValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid server IP address."));
		check(false);
		return;
	}

	// 소켓 생성 (TCP 소켓 사용)
	ClientSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("ClientSocket"), false);

	if (!ClientSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create client socket."));
		check(false);
		return;
	}

	// 소켓 옵션 설정 (논블로킹 모드)
	ClientSocket->SetNonBlocking(true);
	ClientSocket->SetReuseAddr(true);

	// 서버에 연결 시도
	bool bConnected = ClientSocket->Connect(*RemoteAddr);

	if (!bConnected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to connect to server."));
		ClientSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
		check(false);
		return;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Connected to server."));
		// 연결된 소켓을 멤버 변수로 저장하거나 추가적인 처리를 수행
	}
}

void UEngineSocket::RecieveData(const TFunction<void(const Message&)>& Callback)
{
	if (bIsServer)
	{
		OnServerCallback();
		// 서버는 모든 클라이언트 소켓에서 데이터 수신
		for (FSocket* ClientSocket_ : Connections)
		{
			HandleIncomingData(ClientSocket_, Callback);
		}
	}
	else
	{
		// 클라이언트는 서버 소켓에서 데이터 수신
		if (ClientSocket)
		{
			HandleIncomingData(ClientSocket, Callback);
		}
	}
}

void UEngineSocket::HandleIncomingData(FSocket* Socket, const TFunction<void(const Message&)>& Callback)
{
	if (!Socket)
	{
		return;
	}

	uint32 DataSize = 0;
	while (Socket->HasPendingData(DataSize))
	{
		TArray<uint8> Data;
		Data.SetNumUninitialized(FMath::Min(DataSize, 65507u)); // UDP 최대 패킷 크기 제한

		int32 BytesRead = 0;
		if (Socket->Recv(Data.GetData(), Data.Num(), BytesRead))
		{
			if (BytesRead > 0)
			{
				Message RecvMsg = Marshaller::DeserializeMessage(Data);
				Callback(RecvMsg);
			}
		}
	}
}

void UEngineSocket::SendData(Message& Msg)
{
	TArray<uint8> Data = Marshaller::SerializeMessage(Msg);

	if (bIsServer)
	{
		// 서버는 모든 클라이언트에게 데이터 전송
		for (FSocket* ClientSocket_ : Connections)
		{
			int32 BytesSent = 0;
			ClientSocket_->Send(Data.GetData(), Data.Num(), BytesSent);
		}
	}
	else
	{
		// 클라이언트는 서버에게 데이터 전송
		if (ClientSocket)
		{
			int32 BytesSent = 0;
			ClientSocket->Send(Data.GetData(), Data.Num(), BytesSent);
		}
	}
}

void UEngineSocket::SetGameStartCallback(int NumOfPlayers, const TFunction<void()>& Function)
{
	MaxClients = NumOfPlayers;
	GameStartCallback = Function;

	// 테스트용: 플레이어 수가 1명인 경우 바로 게임 시작
	if (NumOfPlayers == 1)
	{
		GameStartCallback();
	}
}

void UEngineSocket::BeginDestroy()
{
	Super::BeginDestroy();
}

void UEngineSocket::AddConnection(FSocket* Connection)
{
	Connections.Add(Connection);

	if (Connections.Num() == MaxClients - 1)
	{
		if (GameStartCallback)
		{
			GameStartCallback();
		}
	}
}

void UEngineSocket::CloseConnection(FSocket* Connection)
{
	if (Connection)
	{
		Connection->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Connection);
		Connections.Remove(Connection);
	}
}

void UEngineSocket::OnServerCallback()
{
	HandleIncomingConnections();
}

void UEngineSocket::OnClientCallback()
{
	// 클라이언트에서는 별도의 콜백 처리 없음
}

void UEngineSocket::HandleIncomingConnections()
{
	if (!ListenSocket)
	{
		return;
	}

	bool bPendingConnection = false;
	ListenSocket->HasPendingConnection(bPendingConnection);

	while (bPendingConnection)
	{
		FSocket* NewClientSocket = ListenSocket->Accept(TEXT("AcceptedConnection"));
		if (NewClientSocket)
		{
			UE_LOG(LogTemp, Log, TEXT("Connection Accepted"));
			AddConnection(NewClientSocket);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to accept connection"));
		}

		ListenSocket->HasPendingConnection(bPendingConnection);
	}
}

void UEngineSocket::SetAsServer()
{
	bIsServer = true;
}

void UEngineSocket::SetAsClient()
{
	bIsServer = false;
}

