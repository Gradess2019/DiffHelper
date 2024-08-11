// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanelTree.h"
#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "SlateOptMacros.h"

#include "UI/SDiffHelperDiffItemContextMenu.h"

#define LOCTEXT_NAMESPACE "DiffPanelTree"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanelTree::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	if (!ensure(Controller.IsValid())) { return; }
	
	STreeView::Construct(
		STreeView<TSharedPtr<FDiffHelperItemNode>>::FArguments()
		.TreeItemsSource(&Controller->GetModel()->DiffPanelData.TreeDiff)
		.SelectionMode(ESelectionMode::SingleToggle)
		.OnSelectionChanged(InArgs._OnSelectionChanged)
		.OnGenerateRow(InArgs._OnGenerateRow)
		.OnContextMenuOpening(InArgs._OnContextMenuOpening)
		.OnGetChildren(this, &SDiffHelperDiffPanelTree::OnGetChildren)
		.OnExpansionChanged(this, &SDiffHelperDiffPanelTree::UpdateExpansionState)
		.OnSetExpansionRecursive(this, &SDiffHelperDiffPanelTree::SetExpansionRecursive)
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column(SDiffHelperDiffPanelConstants::PathColumnId)
			.DefaultLabel(LOCTEXT("PathColumn", "Path"))
			.FillWidth(1.f)
			.SortMode(InArgs._SortMode)
			.OnSort(InArgs._OnSortModeChanged)
		)
	);

	Controller->GetModel()->DiffPanelData.SearchFilter->OnChanged().AddSP(this, &SDiffHelperDiffPanelTree::RequestListRefresh);
}

void SDiffHelperDiffPanelTree::RequestListRefresh()
{
	NeedRestoreExpansion = true;
	STreeView<TSharedPtr<FDiffHelperItemNode>>::RequestListRefresh();
}

void SDiffHelperDiffPanelTree::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	STreeView::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (!IsPendingRefresh() && NeedRestoreExpansion)
	{
		NeedRestoreExpansion = false;
		
		auto NewDirectories = UDiffHelperUtils::GetDirectories(GetRootItems());
		for (const auto& NewDirectory : NewDirectories)
		{
			SetItemExpansion(NewDirectory.Value, NewDirectory.Value->bExpanded);
		}
	}
}

void SDiffHelperDiffPanelTree::OnGetChildren(TSharedPtr<FDiffHelperItemNode> InItem, TArray<TSharedPtr<FDiffHelperItemNode>>& OutChildren)
{
	OutChildren = InItem->Children;
}

void SDiffHelperDiffPanelTree::SetExpansionRecursive(TSharedPtr<FDiffHelperItemNode> InItem, bool bInExpand)
{
	SetItemExpansion(InItem, bInExpand);
	for (const auto& Child : InItem->Children)
	{
		SetExpansionRecursive(Child, bInExpand);
	}
}

void SDiffHelperDiffPanelTree::UpdateExpansionState(TSharedPtr<FDiffHelperItemNode> InItem, bool bInExpand)
{
	InItem->bExpanded = bInExpand;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
