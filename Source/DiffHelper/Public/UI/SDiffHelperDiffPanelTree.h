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

		SLATE_ATTRIBUTE(EColumnSortMode::Type, SortMode)
		
		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
		SLATE_EVENT(FOnGenerateRow, OnGenerateRow)
		SLATE_EVENT(FOnSortModeChanged, OnSortModeChanged)
		

	SLATE_END_ARGS()

private:
	TWeakObjectPtr<UDiffHelperTabController> Controller;
	

public:
	void Construct(const FArguments& InArgs);

private:
	void OnGetChildren(TSharedPtr<FDiffHelperItemNode> InItem, TArray<TSharedPtr<FDiffHelperItemNode>>& OutChildren);
};
