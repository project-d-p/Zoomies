#include "OccupationButton.h"

UOccupationButton::UOccupationButton()
{
	OnClicked.AddDynamic(this, &UOccupationButton::InternalOnClicked);
}

void UOccupationButton::InternalOnClicked()
{
	check(OnClickedCallback)
	OnClickedCallback(Occupation);
}
