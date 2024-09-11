#include "DynamicTextureComponent.h"

#include "FNetLogger.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Rendering/Texture2DResource.h"

void UDynamicTextureComponent::InitializeTexture()
{
	uint32 TotalPixels = TextureWidth * TextureHeight;
	uint32 TextureDataSize = TotalPixels * 4 * sizeof(float);
	TextureData = new uint8[TextureDataSize];

	DynamicTexture = UTexture2D::CreateTransient(TextureWidth, TextureHeight, PF_A32B32G32R32F);
	if (!DynamicTexture)
	{
		return;
	}
	DynamicTexture->CompressionSettings = TC_HDR;
	DynamicTexture->SRGB = 0;
	DynamicTexture->Filter = TF_Nearest;
	DynamicTexture->AddToRoot();
	DynamicTexture->UpdateResource();

	TextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureWidth, TextureHeight);
	if (!TextureRegion)
	{
		return;
	}

	FillTexture(FLinearColor::Red);
	FlushRenderingCommands();
}

void UDynamicTextureComponent::FillTexture(FLinearColor Color)
{
	const uint32 TextureTotalPixels = TextureWidth * TextureHeight;
	float* FloatTextureData = reinterpret_cast<float*>(TextureData);

	for (uint32 i = 0; i < TextureTotalPixels; i++)
	{
		FloatTextureData[i * 4] = Color.R;
		FloatTextureData[i * 4 + 1] = Color.G;
		FloatTextureData[i * 4 + 2] = Color.B;
		FloatTextureData[i * 4 + 3] = Color.A;
	}
}

void UDynamicTextureComponent::UpdateTexture(UTexture2D* inTexture, uint8* InSrcData, bool bFreeData)
{
	if (inTexture == nullptr)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("DynamicTexture is null"));
		return;
	}

	struct FUpdateTextureRegionsData
	{
		FTexture2DResource* Texture2DResource;
		FRHITexture2D* TextureRHI;
		FUpdateTextureRegion2D* Regions;
		uint32 SrcPitch;
		uint32 SrcBpp;
		uint8* SrcData;
	};

	FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
	RegionData->Texture2DResource = static_cast<FTexture2DResource*>(inTexture->GetResource());
	RegionData->TextureRHI = RegionData->Texture2DResource->GetTexture2DRHI();
	RegionData->Regions = TextureRegion;
	RegionData->SrcPitch = TextureWidth * 4 * sizeof(float);
	RegionData->SrcBpp = 4 * sizeof(float);
	RegionData->SrcData = InSrcData;

	if (!RegionData->TextureRHI || !RegionData->SrcData)
	{
		return;
	}
	
	ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)(
		[RegionData, bFreeData](FRHICommandListImmediate& RHICmdList)
		{
			for (uint32 RegionIndex = 0; RegionIndex < 1; ++RegionIndex)
			{
				RHIUpdateTexture2D(
					RegionData->TextureRHI,
					0,
					RegionData->Regions[RegionIndex],
					RegionData->SrcPitch,
					RegionData->SrcData
				);
			}

			if (bFreeData)
			{
				FMemory::Free(RegionData->Regions);
				FMemory::Free(RegionData->SrcData);
			}

			delete RegionData;
		});
	FlushRenderingCommands();
}

bool UDynamicTextureComponent::LoadTextureFromFile(const FString& FilePath)
{
	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
	{
		return false;
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
	
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num()))
	{
		TArray<uint8> RawData;
		if (ImageWrapper->GetRaw(ERGBFormat::RGBAF, 32, RawData))
		{
			int32 ImageWidth = ImageWrapper->GetWidth();
			int32 ImageHeight = ImageWrapper->GetHeight();
	
			if (ImageWidth != TextureWidth || ImageHeight != TextureHeight)
			{
				return false;
			}
	
			const uint32 TextureTotalPixels = TextureWidth * TextureHeight;
			FMemory::Memcpy(TextureData, RawData.GetData(), TextureTotalPixels * 4 * 4);
	
			UpdateTexture(DynamicTexture, TextureData);
			return true;
		}
	}
	return false;
}

TArray64<uint8> UDynamicTextureComponent::CompressTextureDataToEXR()
{
	if (TextureData == nullptr)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("TextureData is null"));
		return TArray64<uint8>();
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);

	if (!ImageWrapper.IsValid())
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Failed to create EXR image wrapper"));
		return TArray64<uint8>();
	}

	ImageWrapper->SetRaw(TextureData, TextureWidth * TextureHeight * 4 * sizeof(float), TextureWidth, TextureHeight, ERGBFormat::RGBAF, 32);
	const TArray64<uint8>& CompressedData = ImageWrapper->GetCompressed();
	return CompressedData;
}

TArray64<uint8> UDynamicTextureComponent::DecompressEXRToRawData(const TArray<uint8>& CompressedData, int32& OutWidth, int32& OutHeight)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);

	if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(CompressedData.GetData(), CompressedData.Num()))
	{
		return TArray64<uint8>();
	}

	TArray64<uint8> RawData;
	if (ImageWrapper->GetRaw(ERGBFormat::RGBAF, 32, RawData))
	{
		OutWidth = ImageWrapper->GetWidth();
		OutHeight = ImageWrapper->GetHeight();
		return RawData;
	}
	return TArray64<uint8>();
}

UDynamicTextureComponent::~UDynamicTextureComponent()
{
	if (TextureData)
	{
		delete[] TextureData;
		TextureData = nullptr;
	}
	
	if (TextureRegion)
	{
		delete TextureRegion;
		TextureRegion = nullptr;
	}
}