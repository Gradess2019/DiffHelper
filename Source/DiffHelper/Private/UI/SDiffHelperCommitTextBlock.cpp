// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperCommitTextBlock.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperCommitTextBlock::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		  .HAlign(HAlign_Left)
		  .VAlign(VAlign_Center)
		  .Padding(4.f, 0.f, 0.f, 0.f)
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
			.Text(InArgs._Text)
			.ToolTipText(InArgs._ToolTip)
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
