// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperCommitContextMenu.h"
#include "DiffHelperCommands.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperCommitContextMenu::Construct(const FArguments& InArgs)
{
	if (!ensure(InArgs._CommandList.IsValid())) { return; }

	FMenuBuilder MenuBuilder(true, InArgs._CommandList);

	MenuBuilder.BeginSection("DiffHelper.CommitPanel.ContextMenu");
	MenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffAgainstTarget);
	MenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffSelectedCommits);
	MenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffSelectedCommitAgainstNext);
	MenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffSelectedCommitAgainstPrevious);
	MenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffSelectedCommitAgainstNewest);
	MenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffSelectedCommitAgainstOldest);
	MenuBuilder.EndSection();

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Content()
		[
			MenuBuilder.MakeWidget()
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
