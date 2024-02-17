// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanel.h"
#include "Widgets/Input/SSearchBox.h"
#include "DiffHelperTypes.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanel::Construct(const FArguments& InArgs)
{
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
	];
}

void SDiffHelperDiffPanel::OnSearchTextChanged(const FText& Text)
{
	UE_LOG(LogDiffHelper, Log, TEXT("Search text changed: %s"), *Text.ToString());
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
