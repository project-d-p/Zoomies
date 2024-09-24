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
	// ��� ����� ���� �ݱ�
	for (FSocket* Socket : Connections)
	{
		if (Socket)
		{
			Socket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		}
	}

	// ���� ���� �ݱ�
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}

	// Ŭ���̾�Ʈ ���� �ݱ�
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
	
	// ���� �ּҿ� ��Ʈ ����
	FString Address = TEXT("0.0.0.0"); // ��� IP���� ����
	int32 Port = SERVER_PORT; // ���ϴ� ��Ʈ ��ȣ�� ����

	// ���� ����ý��� ��������
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	// IP �ּ� ����
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

	// ���� ���� (TCP ���� ���)
	ListenSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("ListenSocket"), false);

	if (!ListenSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create socket."));
		check(false);
		return;
	}

	// ���� �ɼ� ���� (���� �� ����ŷ ���)
	ListenSocket->SetReuseAddr(true);
	ListenSocket->SetNonBlocking(true);

	// ���� ���ε�
	if (!ListenSocket->Bind(*LocalAddr))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to bind socket."));
		CloseConnection(ListenSocket);
		check(false);
		return;
	}

	// ������ ���� (��⿭�� �ִ� ���� ���� 16���� ����)
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
	
	// ���� �ּҿ� ��Ʈ ����
	FString ServerAddress = "127.0.0.1"; // ���� IP �ּҷ� ����
	int32 ServerPort = SERVER_PORT; // ���ϴ� ��Ʈ ��ȣ�� ����

	// ���� ����ý��� ��������
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	// ���� �ּ� ����
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

	// ���� ���� (TCP ���� ���)
	ClientSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("ClientSocket"), false);

	if (!ClientSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create client socket."));
		check(false);
		return;
	}

	// ���� �ɼ� ���� (����ŷ ���)
	ClientSocket->SetNonBlocking(true);
	ClientSocket->SetReuseAddr(true);

	// ������ ���� �õ�
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
		// ����� ������ ��� ������ �����ϰų� �߰����� ó���� ����
	}
}

void UEngineSocket::RecieveData(const TFunction<void(const Message&)>& Callback)
{
	if (bIsServer)
	{
		OnServerCallback();
		// ������ ��� Ŭ���̾�Ʈ ���Ͽ��� ������ ����
		for (FSocket* ClientSocket_ : Connections)
		{
			HandleIncomingData(ClientSocket_, Callback);
		}
	}
	else
	{
		// Ŭ���̾�Ʈ�� ���� ���Ͽ��� ������ ����
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
		Data.SetNumUninitialized(FMath::Min(DataSize, 65507u)); // UDP �ִ� ��Ŷ ũ�� ����

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
		// ������ ��� Ŭ���̾�Ʈ���� ������ ����
		for (FSocket* ClientSocket_ : Connections)
		{
			int32 BytesSent = 0;
			ClientSocket_->Send(Data.GetData(), Data.Num(), BytesSent);
		}
	}
	else
	{
		// Ŭ���̾�Ʈ�� �������� ������ ����
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

	// �׽�Ʈ��: �÷��̾� ���� 1���� ��� �ٷ� ���� ����
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
	// Ŭ���̾�Ʈ������ ������ �ݹ� ó�� ����
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

