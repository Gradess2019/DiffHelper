// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>
#include "DiffHelperTypes.h"

class SDiffHelperBranchPicker;
class UDiffHelperTabController;
/**
 * 
 */
class DIFFHELPER_API SDiffHelperPickerPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperPickerPanel)
			:
			_Controller(nullptr)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)
		SLATE_EVENT(FDiffHelperEvent, OnShowDiff)

	SLATE_END_ARGS()

protected:
	TWeakObjectPtr<UDiffHelperTabController> Controller = nullptr;
	TSharedPtr<SDiffHelperBranchPicker> SourceBranchPicker = nullptr;
	TSharedPtr<SDiffHelperBranchPicker> TargetBranchPicker = nullptr;
	
	FDiffHelperEvent OnShowDiff;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	FReply OnShowDiffClicked() const;
	bool CanShowDiff() const;
};
