// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"
#include "Misc/TextFilter.h"
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
	TSharedPtr<TTextFilter<const FDiffHelperDiffItem&>> SearchFilter;
	TSharedPtr<SSearchBox> SearchBox;
	
	TArray<TSharedPtr<FDiffHelperDiffItem>> OriginalDiff;
	TArray<TSharedPtr<FDiffHelperDiffItem>> FilteredDiff;
	
	TArray<TSharedPtr<FDiffHelperTreeItem>> OriginalTreeDiff;
	TArray<TSharedPtr<FDiffHelperTreeItem>> FilteredTreeDiff;

	FName SortColumn = SDiffHelperDiffPanelConstants::PathColumnId;
	EColumnSortMode::Type SortMode = EColumnSortMode::Ascending;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	EColumnSortMode::Type GetSortModeForColumn(FName InColumnId) const;
	EColumnSortPriority::Type GetSortPriorityForColumn(FName InColumnId) const;

	void PopulateSearchString(const FDiffHelperDiffItem& InItem, TArray<FString>& OutStrings);
	void SortDiffArray(TArray<TSharedPtr<FDiffHelperDiffItem>>& OutArray) const;
	
	void OnSearchTextChanged(const FText& InText);
	void OnFilterChanged();
	void OnSortColumn(EColumnSortPriority::Type InPriority, const FName& InColumnId, EColumnSortMode::Type InSortMode);
	void OnSelectionChanged(TSharedPtr<FDiffHelperDiffItem, ESPMode::ThreadSafe> InSelectedItem, ESelectInfo::Type InSelectType);
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FDiffHelperDiffItem> InItem, const TSharedRef<STableViewBase>& InOwnerTable);
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FDiffHelperTreeItem> InItem, const TSharedRef<STableViewBase>& InOwnerTable);
	void OnGetChildren(TSharedPtr<FDiffHelperTreeItem> DiffHelperTreeItem, TArray<TSharedPtr<FDiffHelperTreeItem>>& Shareds);

};
