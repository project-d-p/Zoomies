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
		// ���� ��Ʈ�ѷ��� ����
		FNetLogger::LogError(TEXT("Keeping existing PlayerController for seamless travel: %s"), *PC->GetName());
        
		// �ʿ��� �ʱ�ȭ�� ����
		InitSeamlessTravelPlayer(C);
		GenericPlayerInitialization(C);
        
		// �� ĳ���� ���� ���� ���� ĳ���� ���� ���� ����
		static int i = 1;
		if (i == 2)
		{
			i = 0;
		}
		FVector Location[2] = {
			{-527.514681,-128.409500,85.462503},
			{-527.514681,138.648437,85.462503}
		};

		FVector SpawnLocation = Location[i++];  // ������ ���� ��ġ ����

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
		// �÷��̾� ��Ʈ�ѷ��� ���� ��쿡 ���� ó��
		Super::HandleSeamlessTravelPlayer(C);
	}
}
