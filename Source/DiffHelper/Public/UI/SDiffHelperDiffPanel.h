// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"
#include "Widgets/SCompoundWidget.h"

struct FDiffHelperDiffItem;
class UDiffHelperTabController;

class DIFFHELPER_API SDiffHelperDiffPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperDiffPanel)
			:
			_Controller(nullptr)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)

	SLATE_END_ARGS()

protected:
	TWeakObjectPtr<UDiffHelperTabController> Controller;
	TSharedPtr<SListView<TSharedPtr<FDiffHelperDiffItem>>> DiffList;
	TArray<TSharedPtr<FDiffHelperDiffItem>> Diff;

	FName SortColumn = SDiffHelperDiffPanelConstants::PathColumnId;
	EColumnSortMode::Type SortMode = EColumnSortMode::Ascending;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	EColumnSortMode::Type GetSortModeForColumn(FName InColumnId) const;
	EColumnSortPriority::Type GetSortPriorityForColumn(FName InColumnId) const;
	
	void OnSearchTextChanged(const FText& InText);
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FDiffHelperDiffItem> InItem, const TSharedRef<STableViewBase>& InOwnerTable);
	void OnSortColumn(EColumnSortPriority::Type InPriority, const FName& InColumnId, EColumnSortMode::Type InSortMode);
};
