#include "CapturedImageWidget.h"

#include "FNetLogger.h"

void UCapturedImageWidget::SetCapturedTexture(UTexture2D* Texture)
{
	if (!Texture)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Texture is null"));
		return;
	}

	if (CapturedImage)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		CapturedImage->SetBrush(Brush);
		FNetLogger::EditerLog(FColor::Cyan, TEXT("SetCapturedTexture Called"));
	}
	else
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("CapturedImage is null"));
	}
}
