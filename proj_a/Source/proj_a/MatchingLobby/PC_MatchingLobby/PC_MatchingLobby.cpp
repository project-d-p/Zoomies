#include "PC_MatchingLobby.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "CineCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "proj_a/MatchingLobby/SteamInvite/SteamInvite.h"
#include "proj_a/MatchingLobby/GM_MatchingLobby/GM_MatchingLobby.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"

APC_MatchingLobby::APC_MatchingLobby()
{
	bReplicates = true; // 클래스 자체가 복제 가능하도록 설정
}

void APC_MatchingLobby::BeginPlay()
{
	Super::BeginPlay();

	ControlledPawn = GetPawn();

	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

}

void APC_MatchingLobby::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 입력 바인딩
	InputComponent->BindAxis("MoveForward", this, &APC_MatchingLobby::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APC_MatchingLobby::MoveRight);
}

void APC_MatchingLobby::MoveForward(float Value)
{
	if (Value != 0.0f && ControlledPawn)
	{
		// 디버그 메시지 출력
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("MoveForward: %f"), Value));
		}

		// 서버에 입력값 전달
		if (IsLocalController())
		{
			ServerMoveForward(Value);
		}

		// 서버라면 직접 이동 처리
		if (GetLocalRole() == ROLE_Authority)
		{
			ControlledPawn->AddMovementInput(FVector::ForwardVector, Value);
		}
	}
}

void APC_MatchingLobby::MoveRight(float Value)
{
	if (Value != 0.0f && ControlledPawn)
	{
		// 디버그 메시지 출력
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("MoveRight: %f"), Value));
		}

		// 서버에 입력값 전달
		if (IsLocalController())
		{
			ServerMoveRight(Value);
		}

		// 서버라면 직접 이동 처리
		if (GetLocalRole() == ROLE_Authority)
		{
			ControlledPawn->AddMovementInput(FVector::RightVector, Value);
		}
	}
}

void APC_MatchingLobby::ServerMoveForward_Implementation(float Value)
{
	MoveForward(Value);
}

bool APC_MatchingLobby::ServerMoveForward_Validate(float Value)
{
	return true;
}

void APC_MatchingLobby::ServerMoveRight_Implementation(float Value)
{
	MoveRight(Value);
}

bool APC_MatchingLobby::ServerMoveRight_Validate(float Value)
{
	return true;
}

void APC_MatchingLobby::ServerSetReady_Implementation(bool pIsReady)
{
	AGS_MatchingLobby* GS_matching_lobby = GetWorld()->GetGameState<AGS_MatchingLobby>();
	AGM_MatchingLobby* GM_matching_lobby = GetWorld()->GetAuthGameMode<AGM_MatchingLobby>();
	
	if (GS_matching_lobby && GM_matching_lobby)
	{
		int32 PlayerIndex = GM_matching_lobby->PCs.Find(this);
		GS_matching_lobby->SetPlayerReady(PlayerIndex, pIsReady);
	}
}

bool APC_MatchingLobby::ServerSetReady_Validate(bool pIsReady)
{
	return true;
}

void APC_MatchingLobby::ToggleReadyState()
{
	bIsReady = !bIsReady;
	ServerSetReady(bIsReady);
}

void APC_MatchingLobby::SetCineCameraView()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACineCameraActor::StaticClass(), FoundActors);

	ACineCameraActor* CineCamera = nullptr;
	for (AActor* Actor : FoundActors)
	{
		if (Actor->GetName() == "CineCameraActor_0")
		{
			CineCamera = Cast<ACineCameraActor>(Actor);
			break;
		}
	}

	if (!CineCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("CineCameraActor not found."));
		return;
	}

	this->SetViewTargetWithBlend(CineCamera);
}

void APC_MatchingLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APC_MatchingLobby, bIsReady);
}
