// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffViewer.h"

#include "DiffHelperSettings.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"
#include "UI/SDiffHelperCommitPanel.h"
#include "UI/SDiffHelperDiffPanel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SDiffHelperDiffViewer::~SDiffHelperDiffViewer()
{
	if (Controller.IsValid())
	{
		Controller->Deinit();
		Controller.Reset();
	}
}

void SDiffHelperDiffViewer::Construct(const FArguments& InArgs)
{
	Controller = NewObject<UDiffHelperTabController>();
	Controller->Init();

	Controller->SetSourceBranch(*InArgs._SourceBranch);
	Controller->SetTargetBranch(*InArgs._TargetBranch);
	
	Controller->CollectDiff();
	
	const auto* Settings = GetDefault<UDiffHelperSettings>();
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SSplitter)
			+ SSplitter::Slot()
			.Value(Settings->DiffPanelRatio)
			.MinSize(Settings->DiffPanelMinSize)
			[
				SNew(SDiffHelperDiffPanel)
				.Controller(Controller)
			]
			+ SSplitter::Slot()
			.Value(Settings->CommitPanelRatio)
			.MinSize(Settings->CommitPanelMinSize)
			[
				SNew(SDiffHelperCommitPanel)
				.Controller(Controller)
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
