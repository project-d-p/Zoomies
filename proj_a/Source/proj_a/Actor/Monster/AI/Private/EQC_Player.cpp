#include "EQC_Player.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "Kismet/GameplayStatics.h"

void UEQC_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(QueryInstance.World, 0);
	if (PlayerController)
	{
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (PlayerPawn)
		{
			UEnvQueryItemType_Point::SetContextHelper(ContextData, PlayerPawn->GetActorLocation());
		}
	}
}
