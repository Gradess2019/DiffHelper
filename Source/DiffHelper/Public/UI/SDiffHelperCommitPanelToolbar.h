// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class DIFFHELPER_API SDiffHelperCommitPanelToolbar : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperCommitPanelToolbar)
		{
		}

	SLATE_END_ARGS()

private:
	TSharedPtr<SButton> DiffSelectedButton;
	TSharedPtr<SButton> DiffAgainstNextButton;
	TSharedPtr<SButton> DiffAgainstPreviousButton;
	TSharedPtr<SButton> DiffAgainstHeadButton;
	TSharedPtr<SButton> DiffAgainstOldestButton;
	

public:
	void Construct(const FArguments& InArgs);
};
