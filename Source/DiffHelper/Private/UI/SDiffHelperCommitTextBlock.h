// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class DIFFHELPER_API SDiffHelperCommitTextBlock : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperCommitTextBlock)
			:
			_Text(FText::GetEmpty()),
			_ToolTip(FText::GetEmpty())
		{
		}

		SLATE_ARGUMENT(FText, Text)
		SLATE_ARGUMENT(FText, ToolTip)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
