// Copyright 2024 Gradess Games. All Rights Reserved.

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

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UDiffHelperSettings, bDevMode))
	{
		if (bDevMode)
		{
			FText Title = FText::FromString("Warning");
			FText Message = FText::FromString("You are about to enable Developer Mode. This will expose advanced settings that can break the plugin. Are you sure you want to continue?");
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
			EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, Message, Title);
#else
			EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, Message, &Title);
#endif
			if (Result == EAppReturnType::No)
			{
				bDevMode = false;
			}
		}
	}

	UObject::PostEditChangeProperty(PropertyChangedEvent);
}
