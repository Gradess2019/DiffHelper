// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UDiffHelperTabController;

class DIFFHELPER_API SDiffHelperCommitContextMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperCommitContextMenu)
		{
		}

		SLATE_ARGUMENT(TSharedPtr<FUICommandList>, CommandList)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
