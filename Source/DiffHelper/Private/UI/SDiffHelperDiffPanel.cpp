﻿// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanel.h"
#include "DiffHelperStyle.h"
#include "DiffHelperTypes.h"
#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"
#include "UI/SDiffHelperDiffPanelList.h"
#include "UI/SDiffHelperDiffPanelTree.h"
#include "UI/SDiffHelperTreeItem.h"
#include "Styling/ToolBarStyle.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/ITableRow.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanel::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	if (!ensure(Controller.IsValid())) { return; }

	Model = Controller->GetModel();
	if (!ensure(Model.IsValid())) { return; }

	DiffList = SNew(SDiffHelperDiffPanelList)
		.Controller(Controller)
		.OnSelectionChanged(this, &SDiffHelperDiffPanel::OnSelectionChanged)
		.OnGenerateRow(this, &SDiffHelperDiffPanel::OnGenerateRow)
		.SortMode(this, &SDiffHelperDiffPanel::GetSortMode)
		.OnSortModeChanged(this, &SDiffHelperDiffPanel::OnSortColumn);

	DiffTree = SNew(SDiffHelperDiffPanelTree)
		.Controller(Controller)
		.OnSelectionChanged(this, &SDiffHelperDiffPanel::OnSelectionChanged)
		.OnGenerateRow(this, &SDiffHelperDiffPanel::OnGenerateRow)
		.SortMode(this, &SDiffHelperDiffPanel::GetSortMode)
		.OnSortModeChanged(this, &SDiffHelperDiffPanel::OnSortColumn);
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f)
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0.f)
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
					.Text(LOCTEXT("DiffPanelTitle", "Diff List"))
				]
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.AutoHeight()
				.Padding(4.f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						
						SNew(SCheckBox)
						.Style(&FAppStyle::GetWidgetStyle<FToolBarStyle>("EditorViewportToolBar").ToggleButton)
						.CheckBoxContentUsesAutoWidth(false)
						.IsFocusable(true)
						.ToolTipText(LOCTEXT("DiffPanelGrouping", "Group by Directory"))		
						.OnCheckStateChanged(this, &SDiffHelperDiffPanel::OnGroupingStateChanged)
						[
							SNew(SImage)
							.Image(FDiffHelperStyle::Get().GetBrush("DiffHelper.Directory"))
						]
					]
					
				]
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.AutoHeight()
				.Padding(4.f)
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
			SNew(SWidgetSwitcher)
			.WidgetIndex(this, &SDiffHelperDiffPanel::GetWidgetIndex)
			+ SWidgetSwitcher::Slot()
			[
				DiffList.ToSharedRef()
			]
			+ SWidgetSwitcher::Slot()
			[
				DiffTree.ToSharedRef()
			]
		]
	];
}

EColumnSortMode::Type SDiffHelperDiffPanel::GetSortMode() const
{
	return Model.IsValid() ? Model->DiffPanelData.SortMode : EColumnSortMode::Ascending;
}

int SDiffHelperDiffPanel::GetWidgetIndex() const
{
	return Model.IsValid() ? Model->DiffPanelData.CurrentWidgetIndex : 0;
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
		.Controller(Controller)
		.Item(InItem);

	if (InOwnerTable == DiffList)
	{
		NewRow->SetToolTipText(FText::FromString(InItem->DiffItem->Path));
	}

	return NewRow;
}

void SDiffHelperDiffPanel::OnGroupingStateChanged(ECheckBoxState CheckBoxState)
{
	Controller->SetActiveWidgetIndex(CheckBoxState == ECheckBoxState::Checked ? 1 : 0);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
