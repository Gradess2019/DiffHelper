// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffViewer.h"

#include "SlateOptMacros.h"

#include "UI/SDiffHelperDiffPanel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffViewer::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			.Value(0.2f)
			[
				SNew(SDiffHelperDiffPanel)
			]
			+ SSplitter::Slot()
			.Value(0.8f)
			[
				SNew(SBorder)
				.ColorAndOpacity(FLinearColor(157, 172, 196))
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
