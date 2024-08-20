// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanelTree.h"
#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "DiffPanelTree"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanelTree::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	if (!ensure(Controller.IsValid())) { return; }

	CanBroadcastSelectionChanged = InArgs._CanBroadcastSelectionChanged;
	
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

#if ENGINE_MAJOR_VERSION > 5 && ENGINE_MINOR_VERSION >= 3
		auto NewDirectories = UDiffHelperUtils::GetDirectories(GetRootItems());
#else
		auto NewDirectories = UDiffHelperUtils::GetDirectories(GetItems());
#endif
		
		for (const auto& NewDirectory : NewDirectories)
		{
			SetItemExpansion(NewDirectory.Value, NewDirectory.Value->bExpanded);
		}
	}
}

void SDiffHelperDiffPanelTree::SetExpansionRecursiveReverse(TSharedPtr<FDiffHelperItemNode> InItem, bool bInExpand)
{
	SetItemExpansion(InItem, bInExpand);
	// find all parents of InItem and expand them, using Directories map, where key - path to dir and value - dir node
#if ENGINE_MAJOR_VERSION > 5 && ENGINE_MINOR_VERSION >= 3
	const auto& Directories = UDiffHelperUtils::GetDirectories(GetRootItems());
#else
	const auto& Directories = UDiffHelperUtils::GetDirectories(GetItems());
#endif

	const auto& ItemPath = InItem->Path;
	TArray<FString> PathComponents;
	ItemPath.ParseIntoArray(PathComponents, TEXT("/"), true);

	for (int32 Id = 0; Id < PathComponents.Num(); Id++)
	{
		// ParentPath work like this:
		// 1 iteration => ParentPath == PathComponents[0]
		// 2 iteration => ParentPath == PathComponents[0] + "/" + PathComponents[1]
		// 3 iteration => ParentPath == PathComponents[0] + "/" + PathComponents[1] + "/" + PathComponents[2]
		FString ParentPath = PathComponents[0];
		for (int32 SubdirId = 1; SubdirId < Id; SubdirId++)
		{
			ParentPath = FPaths::Combine(ParentPath, PathComponents[SubdirId]);
		}

		if (Directories.Contains(ParentPath))
		{
			SetItemExpansion(Directories[ParentPath], bInExpand);
		}
	}
	
}

void SDiffHelperDiffPanelTree::Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo)
{
	if (!CanBroadcastSelectionChanged.IsBound() || CanBroadcastSelectionChanged.Execute(SharedThis(this)))
	{
		STreeView::Private_SignalSelectionChanged(SelectInfo);
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
