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
	// TODO: It is copy-paste code from diff list class. We need to find better solution.
	DECLARE_DELEGATE_RetVal_OneParam(bool, FCanBroadcastSelectionChanged, const TSharedPtr<SListView>&);

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
		SLATE_EVENT(FOnContextMenuOpening, OnContextMenuOpening)
		SLATE_EVENT(FCanBroadcastSelectionChanged, CanBroadcastSelectionChanged)
		
	SLATE_END_ARGS()

private:
	TWeakObjectPtr<UDiffHelperTabController> Controller;
	FCanBroadcastSelectionChanged CanBroadcastSelectionChanged;

	bool NeedRestoreExpansion = false;

public:
	void Construct(const FArguments& InArgs);
	virtual void RequestListRefresh() override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;


protected:
	virtual void Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo) override;

private:
	void OnGetChildren(TSharedPtr<FDiffHelperItemNode> InItem, TArray<TSharedPtr<FDiffHelperItemNode>>& OutChildren);

	void SetExpansionRecursive(TSharedPtr<FDiffHelperItemNode> InItem, bool bInExpand);
	void UpdateExpansionState(TSharedPtr<FDiffHelperItemNode> InItem, bool bInExpand);

	TSharedPtr<SWidget> CreateContextMenu();
};
