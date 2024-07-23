#include "OccupationButton.h"

#include "FNetLogger.h"
#include "PathManager.h"

UOccupationButton::UOccupationButton()
{
	OnClicked.AddDynamic(this, &UOccupationButton::InternalOnClicked);
}

void UOccupationButton::InternalOnClicked()
{
	check(OnClickedCallback)
	OnClickedCallback(Occupation);
}
