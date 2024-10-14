// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>
#include "DiffHelperTypes.h"

class UDiffHelperRevisionPickerController;
class SDiffHelperBranchPicker;

class DIFFHELPER_API SDiffHelperPickerPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperPickerPanel)
		{
		}

		SLATE_EVENT(FDiffHelperEvent, OnShowDiff)

	SLATE_END_ARGS()

	virtual ~SDiffHelperPickerPanel();

protected:
	TWeakObjectPtr<UDiffHelperRevisionPickerController> Controller = nullptr;
	TSharedPtr<SDiffHelperBranchPicker> SourceBranchPicker = nullptr;
	TSharedPtr<SDiffHelperBranchPicker> TargetBranchPicker = nullptr;
	
	FDiffHelperEvent OnShowDiff;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	FReply OnShowDiffClicked() const;
	bool CanShowDiff() const;

	TSharedRef<SDockTab> SpawnTab(const FSpawnTabArgs& InSpawnTabArgs);
	bool CanSpawnTab(const FSpawnTabArgs& InSpawnTabArgs) const;
	TSharedPtr<SDockTab> FindTabToReuse(const FTabId& TabId);

};
