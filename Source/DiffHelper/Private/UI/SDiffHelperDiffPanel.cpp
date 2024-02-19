// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanel.h"
#include "Widgets/Input/SSearchBox.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "SDiffHelperDiffFileItem.h"
#include "SlateOptMacros.h"
#include "Misc/ComparisonUtility.h"
#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/ITableRow.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanel::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	if (!ensure(Controller.IsValid())) { return; }

	const auto* Model = Controller->GetModel();
	Diff = UDiffHelperUtils::ConvertToShared(Model->Diff);

	SortModes.Add(SDiffHelperDiffPanelConstants::StatusColumnId, EColumnSortMode::Ascending);
	SortModes.Add(SDiffHelperDiffPanelConstants::PathColumnId, EColumnSortMode::Ascending);
	SortPriorities.Add(SDiffHelperDiffPanelConstants::StatusColumnId, EColumnSortPriority::Secondary);
	SortPriorities.Add(SDiffHelperDiffPanelConstants::PathColumnId, EColumnSortPriority::Primary);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		  .AutoHeight()
		  .Padding(0.f, 5.f, 0.f, 0.f)
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0.f, 4.f, 0.f, 0.f)
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
					.Text(LOCTEXT("DiffPanelTitle", "Diff List"))
				]
				+ SVerticalBox::Slot()
				  .HAlign(HAlign_Fill)
				  .Padding(4.f, 4.f, 4.f, 4.f)
				[
					SNew(SSearchBox)
					.HintText(LOCTEXT("SearchBoxHint", "Search the files"))
					.OnTextChanged(this, &SDiffHelperDiffPanel::OnSearchTextChanged)
				]
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SAssignNew(DiffList, SListView<TSharedPtr<FDiffHelperDiffItem>>)
			.ListItemsSource(&Diff)
			.OnGenerateRow(this, &SDiffHelperDiffPanel::OnGenerateRow)
			.HeaderRow
			(
				SNew(SHeaderRow)
				+ SHeaderRow::Column(SDiffHelperDiffPanelConstants::StatusColumnId)
				  .DefaultLabel(LOCTEXT("StateColumn", "State"))
				  .FixedWidth(36.f)
				  .SortMode(this, &SDiffHelperDiffPanel::GetSortModeForColumn, SDiffHelperDiffPanelConstants::StatusColumnId)
				  .SortPriority(EColumnSortPriority::Secondary)
				  .OnSort(this, &SDiffHelperDiffPanel::OnSortColumn)
				+ SHeaderRow::Column(SDiffHelperDiffPanelConstants::PathColumnId)
				  .DefaultLabel(LOCTEXT("PathColumn", "Path"))
				  .FillWidth(1.f)
				  .SortMode(this, &SDiffHelperDiffPanel::GetSortModeForColumn, SDiffHelperDiffPanelConstants::PathColumnId)
				  .SortPriority(EColumnSortPriority::Primary)
				  .OnSort(this, &SDiffHelperDiffPanel::OnSortColumn)
			)
		]
	];
}

void SDiffHelperDiffPanel::OnSearchTextChanged(const FText& Text)
{
	UE_LOG(LogDiffHelper, Log, TEXT("Search text changed: %s"), *Text.ToString());
}

TSharedRef<ITableRow> SDiffHelperDiffPanel::OnGenerateRow(TSharedPtr<FDiffHelperDiffItem> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(SDiffHelperDiffFileItem, OwnerTable)
		.Item(Item);
}

EColumnSortMode::Type SDiffHelperDiffPanel::GetSortModeForColumn(FName Name) const
{
	return SortModes.FindRef(Name);
}

void SDiffHelperDiffPanel::OnSortColumn(EColumnSortPriority::Type InPriority, const FName& InColumnId, EColumnSortMode::Type InSortMode)
{
	if (!ensure(SortModes.Contains(InColumnId))) { return; }
	SortModes[InColumnId] = InSortMode;

	auto CompareIcons = [](const TSharedPtr<FDiffHelperDiffItem>& A, const TSharedPtr<FDiffHelperDiffItem>& B)
	{
		return UDiffHelperUtils::CompareStatus(A->Status, B->Status);
	};
	
	auto CompareFiles = [](const TSharedPtr<FDiffHelperDiffItem>& A, const TSharedPtr<FDiffHelperDiffItem>& B)
	{
		return UE::ComparisonUtility::CompareNaturalOrder(A->Path, B->Path) < 0;
	};
	
	auto GetCompareFunc = [this, CompareIcons, CompareFiles](const FName& ColumnId)
	{
		if (ColumnId == SDiffHelperDiffPanelConstants::StatusColumnId)
		{
			return TFunction<bool(const TSharedPtr<FDiffHelperDiffItem>&, const TSharedPtr<FDiffHelperDiffItem>)>(CompareIcons);
		}
		else if (ColumnId == SDiffHelperDiffPanelConstants::PathColumnId)
		{
			return TFunction<bool(const TSharedPtr<FDiffHelperDiffItem>&, const TSharedPtr<FDiffHelperDiffItem>)>(CompareFiles);
		}
	
		ensure(false);
		return TFunction<bool(const TSharedPtr<FDiffHelperDiffItem>&, const TSharedPtr<FDiffHelperDiffItem>)>();
	};

	const auto CompareFunc = GetCompareFunc(InColumnId);
	Diff.Sort([this, InSortMode, &CompareFunc](const TSharedPtr<FDiffHelperDiffItem>& A, const TSharedPtr<FDiffHelperDiffItem>& B)
	{
		const auto Result = CompareFunc(A, B);
		return InSortMode == EColumnSortMode::Ascending ? Result : !Result;
	});
	
	DiffList->RequestListRefresh();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
