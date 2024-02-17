// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanel.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanel::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(STextBlock)
		.Text(FText::FromString("Diff Panel"))
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
