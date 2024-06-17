// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffViewer.h"
#include "SlateOptMacros.h"
#include "UI/SDiffHelperCommitPanel.h"
#include "UI/SDiffHelperDiffPanel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffViewer::Construct(const FArguments& InArgs)
{
	if (!ensure(InArgs._Controller.IsValid())) { return; }
	
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			.Value(0.2f)
			.MinSize(100)
			[
				SNew(SDiffHelperDiffPanel)
				.Controller(InArgs._Controller)
			]
			+ SSplitter::Slot()
			.Value(0.8f)
			.MinSize(200)
			[
				SNew(SDiffHelperCommitPanel)
				.Controller(InArgs._Controller)
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
