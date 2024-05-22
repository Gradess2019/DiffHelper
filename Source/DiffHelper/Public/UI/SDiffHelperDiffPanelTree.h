// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UDiffHelperTabController;
struct FDiffHelperItemNode;


/**
 * 
 */
class DIFFHELPER_API SDiffHelperDiffPanelTree : public STreeView<TSharedPtr<FDiffHelperItemNode>>
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperDiffPanelTree):
			_Controller(nullptr)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)
		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)

	SLATE_END_ARGS()

private:
	TWeakObjectPtr<UDiffHelperTabController> Controller;
	

public:
	void Construct(const FArguments& InArgs);

private:
	void OnGetChildren(TSharedPtr<FDiffHelperItemNode> InItem, TArray<TSharedPtr<FDiffHelperItemNode>>& OutChildren);
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FDiffHelperItemNode> InItem, const TSharedRef<STableViewBase>& InOwnerTable);
};
