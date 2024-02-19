// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	TMap<FName, EColumnSortMode::Type> SortModes;
	TMap<FName, EColumnSortPriority::Type> SortPriorities;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	void OnSearchTextChanged(const FText& Text);
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FDiffHelperDiffItem> Item, const TSharedRef<STableViewBase>& OwnerTable);
	EColumnSortMode::Type GetSortModeForColumn(FName Name) const;
	void OnSortColumn(EColumnSortPriority::Type InPriority, const FName& InColumnId, EColumnSortMode::Type InSortMode);

};
