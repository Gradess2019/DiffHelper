#include "DiffHelperSettings.h"

void UDiffHelperSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UDiffHelperSettings, DiffPanelRatio))
	{
		CommitPanelRatio = 1.f - DiffPanelRatio;
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UDiffHelperSettings, CommitPanelRatio))
	{
		DiffPanelRatio = 1.f - CommitPanelRatio;
	}

	UObject::PostEditChangeProperty(PropertyChangedEvent);
}
