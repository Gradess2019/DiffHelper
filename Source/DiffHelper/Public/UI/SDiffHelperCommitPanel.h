// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"

#include "Widgets/SCompoundWidget.h"

class UDiffHelperTabController;

class DIFFHELPER_API SDiffHelperCommitPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperCommitPanel)
			:
			_Controller(nullptr),
			_Commits()
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)
		SLATE_ARGUMENT(TArray<FDiffHelperCommit>, Commits)

	SLATE_END_ARGS()

protected:
	TWeakObjectPtr<UDiffHelperTabController> Controller;
	TSharedPtr<SListView<TSharedPtr<FDiffHelperCommit>>> CommitList;
	TArray<TSharedPtr<FDiffHelperCommit>> Commits;
	
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	virtual ~SDiffHelperCommitPanel() override;

private:
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FDiffHelperCommit> InItem, const TSharedRef<STableViewBase>& InOwnerTable);
	TSharedPtr<SWidget> OnContextMenuOpening();
	void OnSelectionChanged(TSharedPtr<FDiffHelperCommit> InCommit, ESelectInfo::Type InSelectInfo);
	void OnModelUpdated();
};
