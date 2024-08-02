// Fill out your copyright notice in the Description page of Project Settings.


#include "GM_Lobby.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "proj_a/GameInstance/GI_Zoomies.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"

AGM_Lobby::AGM_Lobby()
{
	PlayerControllerClass = ADPPlayerController::StaticClass();
	DefaultPawnClass = ADPCharacter::StaticClass(); 
}

void AGM_Lobby::BeginPlay()
{
	Super::BeginPlay();

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		if(GameInstance->ResetSession())
		{
			if (UWorld* World = GetWorld())
			{
				TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/widget/WBP_Warning/WBP_Warnging_LostConnection.WBP_Warnging_LostConnection_C"));
				if (WidgetClass)
				{
					if (UUserWidget* WarningWidget = CreateWidget<UUserWidget>(World, WidgetClass))
					{
						WarningWidget->AddToViewport();

						// Set widget to center of the screen
						WarningWidget->SetAnchorsInViewport(FAnchors(0.5f, 0.5f));
						WarningWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));

						// Get viewport size and widget size
						FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
						FVector2D WidgetSize = WarningWidget->GetDesiredSize();

						// Set position to center
						WarningWidget->SetPositionInViewport((ViewportSize - WidgetSize) / 2, true);

						FTimerHandle TimerHandle;
						GetWorld()->GetTimerManager().SetTimer(
							TimerHandle,
							FTimerDelegate::CreateLambda([WarningWidget]()
							{
								if (WarningWidget)
								{
									WarningWidget->RemoveFromParent();
								}
							}),
							3.0f, // Time to wait before executing the delegate
							false // Execute only once
						);
					}
				}
			}
		}
	}
}

