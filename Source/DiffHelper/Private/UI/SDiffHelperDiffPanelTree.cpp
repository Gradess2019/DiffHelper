// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanelTree.h"
#include "DiffHelperTypes.h"

#include "SDiffHelperTreeItem.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"

#define LOCTEXT_NAMESPACE "DiffPanelTree"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanelTree::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	if (!ensure(Controller.IsValid())) { return; }
	
	STreeView::Construct(
		STreeView<TSharedPtr<FDiffHelperItemNode>>::FArguments()
		.TreeItemsSource(&Controller->GetModel()->DiffPanelData.FilteredTreeDiff)
		.SelectionMode(ESelectionMode::SingleToggle)
		.OnSelectionChanged(InArgs._OnSelectionChanged)
		.OnGenerateRow(InArgs._OnGenerateRow)
		.OnGetChildren(this, &SDiffHelperDiffPanelTree::OnGetChildren)
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column(SDiffHelperDiffPanelConstants::PathColumnId)
			.DefaultLabel(LOCTEXT("PathColumn", "Path"))
			.FillWidth(1.f)
		)
	);

	Controller->GetModel()->DiffPanelData.SearchFilter->OnChanged().AddSP(this, &SDiffHelperDiffPanelTree::RequestListRefresh);
}

void SDiffHelperDiffPanelTree::OnGetChildren(TSharedPtr<FDiffHelperItemNode> InItem, TArray<TSharedPtr<FDiffHelperItemNode>>& OutChildren)
{
	OutChildren = InItem->Children;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
