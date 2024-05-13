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
	OriginalDiff = FilteredDiff = UDiffHelperUtils::ConvertToShared(Model->Diff);

	SearchFilter = MakeShared<TTextFilter<const FDiffHelperDiffItem&>>(TTextFilter<const FDiffHelperDiffItem&>::FItemToStringArray::CreateSP(this, &SDiffHelperDiffPanel::PopulateSearchString));
	SearchFilter->OnChanged().AddSP(this, &SDiffHelperDiffPanel::OnFilterChanged);

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
			SAssignNew(DiffList, SListView<TSharedPtr<FDiffHelperDiffItem>>)
			.ListItemsSource(&FilteredDiff)
			.SelectionMode(ESelectionMode::SingleToggle)
			.OnSelectionChanged(this, &SDiffHelperDiffPanel::OnSelectionChanged)
			.OnGenerateRow(this, &SDiffHelperDiffPanel::OnGenerateRow)
			.HeaderRow
			(
				SNew(SHeaderRow)
				+ SHeaderRow::Column(SDiffHelperDiffPanelConstants::StatusColumnId)
				  .DefaultLabel(FText::GetEmpty())
				  .FixedWidth(28.f)
				  .SortMode(this, &SDiffHelperDiffPanel::GetSortModeForColumn, SDiffHelperDiffPanelConstants::StatusColumnId)
				  .SortPriority(this, &SDiffHelperDiffPanel::GetSortPriorityForColumn, SDiffHelperDiffPanelConstants::StatusColumnId)
				  .OnSort(this, &SDiffHelperDiffPanel::OnSortColumn)
				+ SHeaderRow::Column(SDiffHelperDiffPanelConstants::PathColumnId)
				  .DefaultLabel(LOCTEXT("PathColumn", "Path"))
				  .FillWidth(1.f)
				  .SortMode(this, &SDiffHelperDiffPanel::GetSortModeForColumn, SDiffHelperDiffPanelConstants::PathColumnId)
				  .SortPriority(this, &SDiffHelperDiffPanel::GetSortPriorityForColumn, SDiffHelperDiffPanelConstants::StatusColumnId)
				  .OnSort(this, &SDiffHelperDiffPanel::OnSortColumn)
			)
		]
	];
}

EColumnSortMode::Type SDiffHelperDiffPanel::GetSortModeForColumn(FName InColumnId) const
{
	if (InColumnId == SortColumn)
	{
		return SortMode;
	}

	return EColumnSortMode::None;
}

EColumnSortPriority::Type SDiffHelperDiffPanel::GetSortPriorityForColumn(FName InColumnId) const
{
	if (InColumnId == SortColumn)
	{
		return EColumnSortPriority::Primary;
	}

	return EColumnSortPriority::None;
}

void SDiffHelperDiffPanel::PopulateSearchString(const FDiffHelperDiffItem& InItem, TArray<FString>& OutStrings)
{
	OutStrings.Add(InItem.Path);
}

void SDiffHelperDiffPanel::SortDiffArray(TArray<TSharedPtr<FDiffHelperDiffItem>>& OutArray) const
{
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

	const auto CompareFunc = GetCompareFunc(SortColumn);
	OutArray.Sort([this, &CompareFunc](const TSharedPtr<FDiffHelperDiffItem>& A, const TSharedPtr<FDiffHelperDiffItem>& B)
	{
		const auto Result = CompareFunc(A, B);
		return SortMode == EColumnSortMode::Ascending ? Result : !Result;
	});
}

void SDiffHelperDiffPanel::OnSearchTextChanged(const FText& InText)
{
	UE_LOG(LogDiffHelper, Log, TEXT("Search text changed: %s"), *InText.ToString());
	SearchFilter->SetRawFilterText(InText);
	SearchBox->SetError(SearchFilter->GetFilterErrorText());
}

void SDiffHelperDiffPanel::OnFilterChanged()
{
	FilteredDiff = OriginalDiff;
	FilteredDiff.RemoveAll([this](const TSharedPtr<FDiffHelperDiffItem>& InItem)
	{
		return !SearchFilter->PassesFilter(*InItem);
	});

	SortDiffArray(FilteredDiff);

	DiffList->RequestListRefresh();
}

void SDiffHelperDiffPanel::OnSortColumn(EColumnSortPriority::Type InPriority, const FName& InColumnId, EColumnSortMode::Type InSortMode)
{
	SortColumn = InColumnId;
	SortMode = InSortMode;

	SortDiffArray(FilteredDiff);

	DiffList->RequestListRefresh();
}

void SDiffHelperDiffPanel::OnSelectionChanged(TSharedPtr<FDiffHelperDiffItem, ESPMode::ThreadSafe> InSelectedItem, ESelectInfo::Type InSelectType)
{
	if (InSelectedItem.IsValid())
	{
		Controller->SelectDiffItem(*InSelectedItem);
	}
	else
	{
		Controller->SelectDiffItem(FDiffHelperDiffItem());
	}
	
	Controller->CallModelUpdated();
}

TSharedRef<ITableRow> SDiffHelperDiffPanel::OnGenerateRow(TSharedPtr<FDiffHelperDiffItem> InItem, const TSharedRef<STableViewBase>& InOwnerTable)
{
	return
		SNew(SDiffHelperDiffFileItem, InOwnerTable)
		.Item(InItem);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
