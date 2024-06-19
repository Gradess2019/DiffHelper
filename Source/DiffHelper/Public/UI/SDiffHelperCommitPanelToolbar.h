// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UDiffHelperTabModel;
class UDiffHelperTabController;
struct FDiffHelperCommit;

class DIFFHELPER_API SDiffHelperCommitPanelToolbar : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperCommitPanelToolbar) :
			_Controller(nullptr)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)

	SLATE_END_ARGS()

private:
	TSharedPtr<SButton> DiffSelectedButton;
	TSharedPtr<SButton> DiffAgainstNextButton;
	TSharedPtr<SButton> DiffAgainstPreviousButton;
	TSharedPtr<SButton> DiffAgainstHeadButton;
	TSharedPtr<SButton> DiffAgainstOldestButton;

	TWeakObjectPtr<UDiffHelperTabController> Controller;
	TWeakObjectPtr<const UDiffHelperTabModel> Model;

public:
	void Construct(const FArguments& InArgs);

private:
	int32 GetCommitIndex(const FDiffHelperCommit& InCommit) const;
	
	void OnModelUpdated();
	
	bool IsDiffSelectedButtonEnabled() const;
	bool HasNextCommit() const;
	bool HasPreviousCommit() const;
};
