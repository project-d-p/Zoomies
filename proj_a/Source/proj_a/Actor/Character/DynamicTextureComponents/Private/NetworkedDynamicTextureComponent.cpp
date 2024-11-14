#include "NetworkedDynamicTextureComponent.h"

#include "FNetLogger.h"
#include "JudgeGameMode.h"
#include "JudgeGameState.h"
#include "JudgePlayerController.h"
#include "TextureTransferManager.h"
#include "Animation/SkeletalMeshActor.h"
#include "Kismet/GameplayStatics.h"

bool UNetworkedDynamicTextureComponent::InitializeTexture(FNetworkedDynamicTextureComponentInitializer& Initializer)
{
	if (!IsValid(Initializer.DynamicMaterialInstance) || !IsValid(Initializer.SkeletalMeshComponent)
		|| !IsValid(Initializer.PlayerState) || !IsValid(Initializer.TextureTransferManager))
	{
		return false;
	}
	
	DynamicMaterialInstance = Initializer.DynamicMaterialInstance;
	SkeletalMeshComponent = Initializer.SkeletalMeshComponent;
	PlayerState = Initializer.PlayerState;
	TextureTransferManager = Initializer.TextureTransferManager;
	if (!PlayerState || !TextureTransferManager)
	{
		FNetLogger::LogError(TEXT("Failed to initialize texture in 'UNetworkedDynamicTextureComponent'."));
		return false;	
	}

	UDynamicTextureComponent::InitializeTexture();
	return true;
}

void UNetworkedDynamicTextureComponent::OnTransferComplete(const TArray<uint8>& FullData)
{
	if (FullData.Num() > 0)
	{
		ADynamicTexturedCharacter* C = Cast<ADynamicTexturedCharacter>(GetOwner());
		if (!C)
			return;
		
		FSerializedTextureData STD;
		STD.CompressedTextureData = FullData;
		STD.Width = 1024;
		STD.Height = 1024;
		UTexture2D* Texture = DeserializeTexture(STD);
		if (!Texture)
			return;
		GetWorld()->GetTimerManager().ClearTimer(C->TimerHandle_InitializeDynamicTexture);
		UpdateTexture(Texture);
	}
}

FSerializedTextureData UNetworkedDynamicTextureComponent::SerializeTexture(UTexture2D* Texture)
{
	FSerializedTextureData OutData;
	if (Texture && Texture->GetPlatformData())
	{
		FTexturePlatformData* PlatformData = Texture->GetPlatformData();
		FTexture2DMipMap* Mip = &PlatformData->Mips[0];

		OutData.Width = Mip->SizeX;
		OutData.Height = Mip->SizeY;

		FByteBulkData* RawImageData = &Mip->BulkData;
		FLinearColor* RawImageDataPtr = static_cast<FLinearColor*>( RawImageData->Lock( LOCK_READ_ONLY ) );
		// const void* DataPointer = Mip.BulkData.Lock(LOCK_READ_ONLY);
		if (RawImageDataPtr)
		{
			// uint8* dd =  DynamicTextureComponent->TextureData;
			const TArray64<uint8>& dd = CompressTextureDataToEXR();
			const int32 BytesPerPixel = GPixelFormats[PlatformData->PixelFormat].BlockBytes;
			const int32 DataSize = Texture->GetSizeX() * Texture->GetSizeY() * BytesPerPixel;
			// const int32 DataSize = 1024;
			OutData.CompressedTextureData.SetNumUninitialized(dd.Num());
			FMemory::Memcpy(OutData.CompressedTextureData.GetData(), dd.GetData(), dd.Num());
			Mip->BulkData.Unlock();
		}

		// void* TextureDataPointer = Mip->BulkData.Lock(LOCK_READ_WRITE);
		// if (TextureDataPointer)
		// {
		// 	TArray64<uint8> dd = DynamicTextureComponent->DecompressEXRToRawData(OutData.CompressedTextureData, OutData.Width, OutData.Height);
		// 	// FMemory::Memzero(TextureDataPointer, Mip.SizeX * Mip.SizeY * 16);
		// 	FMemory::Memcpy(TextureDataPointer, dd.GetData(), dd.Num());
		// 	Mip->BulkData.Unlock();
		// }
		
		// UpdateTexture(Texture);
	}
	return OutData;
}

void UNetworkedDynamicTextureComponent::LoadTexture()
{
	const FString FilePath = FPaths::ProjectContentDir() + TEXT("customCharacter/customImage2.png");
	LoadTextureFromFile(FilePath);
	UTexture2D* CustomTexture = GetDynamicTexture();
		
	if (!CustomTexture)
	{
		FNetLogger::LogError(TEXT("Failed to load texture in 'UNetworkedDynamicTextureComponent'."));
		return ;
	}

	if (GetOwner()->HasLocalNetOwner())
	{
		HandleLocalNetOwner(CustomTexture);
	}
	else
	{
		HandleRemoteNetOwner();
	}
}

void UNetworkedDynamicTextureComponent::HandleLocalNetOwner(UTexture2D* CustomTexture)
{
	ADynamicTexturedCharacter* C = Cast<ADynamicTexturedCharacter>(GetOwner());
	if (!C)
		return;
	GetWorld()->GetTimerManager().ClearTimer(C->TimerHandle_InitializeDynamicTexture);
	FNetLogger::EditerLog(FColor::Cyan, TEXT("Transfer Complete."));
	if (GetWorld()->GetNetMode() != NM_Standalone)
	{
		if (GetOwner()->HasAuthority())
		{
			bCustomTextureUploaded = true;
		}
		else
		{
			FDataTransferParams Params(SerializeTexture(CustomTexture).CompressedTextureData, PlayerState->GetPlayerId());
			TextureTransferManager->SendLargeDataInChunks(Params);
		}
	}
	UpdateTexture(CustomTexture);
}

void UNetworkedDynamicTextureComponent::HandleRemoteNetOwner()
{
	if (!GetOwner()->HasAuthority())
	{
		FNetLogger::EditerLog(FColor::Yellow, TEXT("Request %d texture to server"), PlayerState->GetPlayerId());
		TextureTransferManager->RequestTextureToServer(PlayerState->GetPlayerId());
	}
}

UTexture2D* UNetworkedDynamicTextureComponent::DeserializeTexture(FSerializedTextureData& InData)
{
	if (InData.CompressedTextureData.Num() == 0 || InData.Width <= 0 || InData.Height <= 0)
	{
		FNetLogger::LogError(TEXT("Failed to deserialize texture in 'UNetworkedDynamicTextureComponent'."));
		return nullptr;
	}
	
	UTexture2D* Texture = GetDynamicTexture();
	if (!Texture)
	{
		return nullptr;
	}

	FTexturePlatformData* PlatformData = Texture->GetPlatformData();
	FTexture2DMipMap* Mip = &PlatformData->Mips[0];

	Mip->SizeX = InData.Width;
	Mip->SizeY = InData.Height;

	void* TextureDataPointer = Mip->BulkData.Lock(LOCK_READ_WRITE);
	if (TextureDataPointer)
	{
		TArray64<uint8> dd = DecompressEXRToRawData(InData.CompressedTextureData, InData.Width, InData.Height);
		// Udpate Texture data
		delete TextureData;
		TextureData = new uint8[dd.Num()];
		FMemory::Memcpy(TextureData, dd.GetData(), dd.Num());
		FMemory::Memcpy(TextureDataPointer, dd.GetData(), dd.Num());
		Mip->BulkData.Unlock();
	}
	Texture->UpdateResource();
	
	return Texture;
}

void UNetworkedDynamicTextureComponent::UpdateTexture(UTexture2D* NewTexture)
{
	FString LevelName = GetWorld()->GetMapName();
	if (LevelName.Contains("judge"))
	{
		AJudgeGameState* GS = Cast<AJudgeGameState>(GetWorld()->GetGameState());
		if (!GS)
		{
			FNetLogger::LogError(TEXT("Failed to get JudgeGameState in 'UNetworkedDynamicTextureComponent'."));
			return;
		}

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASkeletalMeshActor::StaticClass(), FoundActors);

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AJudgePlayerController* JPC = Cast<AJudgePlayerController>(*It);
			if (!JPC)
				continue;

			if (GS->GS_PlayerData.Num() > 0)
			{
				int32 PlayerIndex = 0;
				
				for (FPlayerInitData& PlayerData : GS->GS_PlayerData)
				{
					if (PlayerData.PlayerId == PlayerState->GetPlayerId())
					{
						PlayerIndex = PlayerData.CameraIndex;
						GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("PlayerIndex: %d is Loaded!"), PlayerIndex));
						break;
					}
				}
				if (PlayerIndex >= GS->GS_PlayerData.Num())
				{
					FNetLogger::LogError(TEXT("Failed to find player data in 'UNetworkedDynamicTextureComponent'."));
					return;
				}

				for (AActor* Actor : FoundActors)
				{
					// FString ActorName = Actor->GetActorLabel();
					FString ActorName = Actor->GetActorNameOrLabel();
					if (ActorName == FString::Printf(TEXT("Character_%d"), PlayerIndex))
					{
						ASkeletalMeshActor* SkeletalMeshActor = Cast<ASkeletalMeshActor>(Actor);
						if (SkeletalMeshActor)
						{
							SkeletalMeshComponent = SkeletalMeshActor->GetSkeletalMeshComponent();
							if (DynamicMaterialInstance && NewTexture)
							{
								DynamicMaterialInstance->SetTextureParameterValue(TEXT("renderTarget"), NewTexture);
								SkeletalMeshComponent->SetMaterial(0, DynamicMaterialInstance);
							}
						}
						break;
					}
				}
			}
		}
	}
	if (NewTexture && DynamicMaterialInstance && SkeletalMeshComponent)
	{
		DynamicMaterialInstance->SetTextureParameterValue(TEXT("renderTarget"), NewTexture);
		SkeletalMeshComponent->SetMaterial(0, DynamicMaterialInstance);
	}
	else
	{
		FNetLogger::LogError(TEXT("Failed to update texture in 'UNetworkedDynamicTextureComponent'."));
	}
}
