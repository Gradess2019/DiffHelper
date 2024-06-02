// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanel.h"
#include "Widgets/Input/SSearchBox.h"
#include "DiffHelperTypes.h"
#include "SlateOptMacros.h"
#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"
#include "UI/SDiffHelperDiffPanelList.h"
#include "UI/SDiffHelperDiffPanelTree.h"
#include "UI/SDiffHelperTreeItem.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/ITableRow.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanel::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	if (!ensure(Controller.IsValid())) { return; }

	Model = Controller->GetModel();
	if (!ensure(Model.IsValid())) { return; }


	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 0.f, 0.f, 0.f)
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0.f, 0.f, 0.f, 0.f)
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
					.Text(LOCTEXT("DiffPanelTitle", "Diff List"))
				]
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.Padding(4.f, 4.f, 4.f, 4.f)
				[
					SAssignNew(SearchBox, SSearchBox)
					.HintText(LOCTEXT("SearchBoxHint", "Search the files"))
					.OnTextChanged(this, &SDiffHelperDiffPanel::OnSearchTextChanged)
				]
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SAssignNew(DiffList, SDiffHelperDiffPanelList)
			.Controller(Controller)
			.OnSelectionChanged(this, &SDiffHelperDiffPanel::OnSelectionChanged)
			.OnGenerateRow(this, &SDiffHelperDiffPanel::OnGenerateRow)
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SNew(SDiffHelperDiffPanelTree)
			.Controller(Controller)
			.OnSelectionChanged(this, &SDiffHelperDiffPanel::OnSelectionChanged)
			.OnGenerateRow(this, &SDiffHelperDiffPanel::OnGenerateRow)
		]
	];
}

EColumnSortMode::Type SDiffHelperDiffPanel::GetSortModeForColumn(FName InColumnId) const
{
	if (InColumnId == Model->DiffPanelData.SortColumn)
	{
		return Model->DiffPanelData.SortMode;
	}

	return EColumnSortMode::None;
}

EColumnSortPriority::Type SDiffHelperDiffPanel::GetSortPriorityForColumn(FName InColumnId) const
{
	if (InColumnId == Model->DiffPanelData.SortColumn)
	{
		return EColumnSortPriority::Primary;
	}

	return EColumnSortPriority::None;
}

void SDiffHelperDiffPanel::OnSearchTextChanged(const FText& InText)
{
	Controller->SetSearchFilter(InText);

	const auto Error = Model->DiffPanelData.SearchFilter->GetFilterErrorText();
	SearchBox->SetError(Error);
}

void SDiffHelperDiffPanel::OnSortColumn(EColumnSortPriority::Type InPriority, const FName& InColumnId, EColumnSortMode::Type InSortMode)
{
	Controller->SetSortingMode(InColumnId, InSortMode);

	if (DiffList.IsValid())
	{
		DiffList->RequestListRefresh();
	}

	if (DiffTree.IsValid())
	{
		DiffTree->RequestTreeRefresh();
	}
}

void SDiffHelperDiffPanel::OnSelectionChanged(TSharedPtr<FDiffHelperItemNode> InSelectedItem, ESelectInfo::Type InSelectType)
{
	if (InSelectedItem.IsValid() && InSelectedItem->DiffItem.IsValid())
	{
		Controller->SelectDiffItem(*InSelectedItem->DiffItem);
	}
	else
	{
		Controller->SelectDiffItem(FDiffHelperDiffItem());
	}

	Controller->CallModelUpdated();
}

TSharedRef<ITableRow> SDiffHelperDiffPanel::OnGenerateRow(TSharedPtr<FDiffHelperItemNode> InItem, const TSharedRef<STableViewBase>& InOwnerTable)
{
	auto NewRow = SNew(SDiffHelperTreeItem, InOwnerTable)
		.Item(InItem);

	if (InOwnerTable == DiffList)
	{
		NewRow->SetToolTipText(FText::FromString(InItem->DiffItem->Path));
	}
	
	return NewRow;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
