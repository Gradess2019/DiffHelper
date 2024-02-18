// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanel.h"
#include "Widgets/Input/SSearchBox.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "SlateOptMacros.h"
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
			SNew(SListView<TSharedPtr<FDiffHelperDiffItem>>)
			.ItemHeight(24.f)
			.ListItemsSource(&Diff)
			.OnGenerateRow(this, &SDiffHelperDiffPanel::OnGenerateRow)
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
		SNew(STableRow<TSharedRef<FDiffHelperDiffItem>>, OwnerTable)
		[
			SNew(STextBlock).Text(FText::FromString(Item->Path))
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
