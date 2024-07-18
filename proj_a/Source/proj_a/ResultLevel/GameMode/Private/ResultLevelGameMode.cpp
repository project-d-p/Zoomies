#include "ResultLevelGameMode.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "LocalizationDescriptor.h"
#include "ResultLevelGameState.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AResultLevelGameMode::AResultLevelGameMode()
{
	bUseSeamlessTravel = true;

	PrimaryActorTick.bCanEverTick = true;
	
	DefaultPawnClass = ADPCharacter::StaticClass();
	PlayerControllerClass = ADPPlayerController::StaticClass();
	PlayerStateClass = ADPPlayerState::StaticClass();
	GameStateClass = AResultLevelGameState::StaticClass();
}

AResultLevelGameMode::~AResultLevelGameMode()
{
}

/* Seamless Travel : Reuse PlayerController for Server */
void AResultLevelGameMode::PostSeamlessTravel()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (!GameInstance) return;

	FVector Location[2] = {
		{-527.514681,-128.409500,85.462503},
		{-527.514681,138.648437,85.462503}
	};

	ADPCharacter* NewCharacter = GetWorld()->SpawnActor<ADPCharacter>(DefaultPawnClass, Location[0], FRotator::ZeroRotator);
	ADPPlayerController* PlayerController = Cast<ADPPlayerController>(GameInstance->LocalController);
	
	if (PlayerController && PlayerController->IsLocalPlayerController())
	{
		if (!NewCharacter)
		{
			return ;
		}
		PlayerController->Possess(NewCharacter);
		PlayerController->SwitchLevelComponent(ELevelComponentType::RESULT);
	}
}

void AResultLevelGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

/* Seamless Travel : Reuse PlayerController for Clients */
void AResultLevelGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	APlayerController* PC = Cast<APlayerController>(C);
	FNetLogger::LogError(TEXT("Handling seamless travel for: %d"), PC->GetUniqueID());
	if (PC && PC->Player)
	{
		// 기존 컨트롤러를 유지
		FNetLogger::LogError(TEXT("Keeping existing PlayerController for seamless travel: %s"), *PC->GetName());
        
		// 필요한 초기화만 수행
		InitSeamlessTravelPlayer(C);
		GenericPlayerInitialization(C);
        
		// 새 캐릭터 생성 없이 기존 캐릭터 재사용 로직 구현
		static int i = 1;
		if (i == 2)
		{
			i = 0;
		}
		FVector Location[2] = {
			{-527.514681,-128.409500,85.462503},
			{-527.514681,138.648437,85.462503}
		};

		FVector SpawnLocation = Location[i++];  // 적절한 스폰 위치 설정

		ADPCharacter* NewCharacter = GetWorld()->SpawnActor<ADPCharacter>(DefaultPawnClass, SpawnLocation, FRotator::ZeroRotator);
		if (NewCharacter)
		{
			PC->Possess(NewCharacter);
		}

		ADPPlayerController* PlayerController = Cast<ADPPlayerController>(PC);
		if (PlayerController)
		{
			PlayerController->SwitchLevelComponent(ELevelComponentType::RESULT);
		}
	}
	else
	{
		// 플레이어 컨트롤러가 없는 경우에 대한 처리
		Super::HandleSeamlessTravelPlayer(C);
	}
}
