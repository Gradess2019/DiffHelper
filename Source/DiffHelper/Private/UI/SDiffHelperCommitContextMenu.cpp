// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperCommitContextMenu.h"
#include "DiffHelperCommands.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperCommitContextMenu::BuildMenu(FMenuBuilder& InMenuBuilder)
{
	InMenuBuilder.BeginSection("DiffHelper.CommitPanel.ContextMenu");
	InMenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffAgainstTarget);
	InMenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffSelectedCommits);
	InMenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffSelectedCommitAgainstNext);
	InMenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffSelectedCommitAgainstPrevious);
	InMenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffSelectedCommitAgainstNewest);
	InMenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffSelectedCommitAgainstOldest);
	InMenuBuilder.EndSection();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
