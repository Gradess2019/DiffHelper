// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanelList.h"
#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "DiffHelperDiffPanelList"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanelList::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	if (!ensure(Controller.IsValid())) { return; }

	Model = Controller->GetModel();
	if (!ensure(Model.IsValid())) { return; }

	CanBroadcastSelectionChanged = InArgs._CanBroadcastSelectionChanged;

	SListView::Construct(
		SListView::FArguments()
		.ListItemsSource(&Model->DiffPanelData.FilteredDiff)
		.SelectionMode(ESelectionMode::SingleToggle)
		.OnSelectionChanged(InArgs._OnSelectionChanged)
		.OnGenerateRow(InArgs._OnGenerateRow)
		.OnContextMenuOpening(InArgs._OnContextMenuOpening)
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
}

void SDiffHelperDiffPanelList::Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo)
{
	if (!CanBroadcastSelectionChanged.IsBound() || CanBroadcastSelectionChanged.Execute(SharedThis(this)))
	{
		SListView::Private_SignalSelectionChanged(SelectInfo);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE