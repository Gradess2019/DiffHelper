// Copyright 2024 Gradess Games. All Rights Reserved.


#include "SDiffHelperDiffPanelList.h"
#include "SlateOptMacros.h"
#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"

#define LOCTEXT_NAMESPACE "DiffHelperDiffPanelList"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanelList::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	if (!ensure(Controller.IsValid())) { return; }

	Model = Controller->GetModel();
	if (!ensure(Model.IsValid())) { return; }

	// ==================================
	// TODO: This is uncompileable comment.
	// Next task:
	// - Think about sort, probably we don't need it
	// ==================================
	
	SListView::Construct(
		SListView::FArguments()
		.ListItemsSource(&Model->DiffPanelData.FilteredDiff)
		.SelectionMode(ESelectionMode::SingleToggle)
		.OnSelectionChanged(InArgs._OnSelectionChanged)
		.OnGenerateRow(InArgs._OnGenerateRow)
		.HeaderRow
		(
			SNew(SHeaderRow)
			// + SHeaderRow::Column(SDiffHelperDiffPanelConstants::StatusColumnId)
			// .DefaultLabel(FText::GetEmpty())
			// .FixedWidth(28.f)
			// .SortMode(this, &SDiffHelperDiffPanel::GetSortModeForColumn, SDiffHelperDiffPanelConstants::StatusColumnId)
			// .SortPriority(this, &SDiffHelperDiffPanel::GetSortPriorityForColumn, SDiffHelperDiffPanelConstants::StatusColumnId)
			// .OnSort(this, &SDiffHelperDiffPanel::OnSortColumn)
			+ SHeaderRow::Column(SDiffHelperDiffPanelConstants::PathColumnId)
			.DefaultLabel(LOCTEXT("PathColumn", "Path"))
			.FillWidth(1.f)
			// .SortMode(this, &SDiffHelperDiffPanel::GetSortModeForColumn, SDiffHelperDiffPanelConstants::PathColumnId)
			// .SortPriority(this, &SDiffHelperDiffPanel::GetSortPriorityForColumn, SDiffHelperDiffPanelConstants::StatusColumnId)
			// .OnSort(this, &SDiffHelperDiffPanel::OnSortColumn)
		)
	);

	Model->DiffPanelData.SearchFilter->OnChanged().AddSP(this, &SListView::RequestListRefresh);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE