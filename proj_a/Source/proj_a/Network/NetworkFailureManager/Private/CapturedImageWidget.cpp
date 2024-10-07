#include "CapturedImageWidget.h"

void UCapturedImageWidget::SetCapturedTexture(UTexture2D* Texture)
{
	if (CapturedImage && Texture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		CapturedImage->SetBrush(Brush);
	}
}
