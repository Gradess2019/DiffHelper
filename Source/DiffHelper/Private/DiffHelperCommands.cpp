// Copyright Epic Games, Inc. All Rights Reserved.

#include "DiffHelperCommands.h"

#define LOCTEXT_NAMESPACE "FDiffHelperModule"

void FDiffHelperCommands::RegisterCommands()
{
	UI_COMMAND(OpenDiffWindow, "DiffHelper", "Open diff helper window", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CreateNewDiff, "New...", "Create a new diff", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(GroupByDirectory, "Group By Directory", "Group diff items by directory", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(ExpandAll, "Expand All", "Expand all diff items", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CollapseAll, "Collapse All", "Collapse all diff items", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(DiffAgainstTarget, "Diff Against Target", "Diff file against the target branch", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DiffSelectedCommits, "Diff Selected", "Diff selected commits against each other", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DiffSelectedCommitAgainstNext, "Diff Selected Against Next", "Diff selected commit against the next one", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DiffSelectedCommitAgainstPrevious, "Diff Selected Against Previous", "Diff selected commit against the previous one", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DiffSelectedCommitAgainstNewest, "Diff Selected Against Newest", "Diff selected commit against the newest one", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DiffSelectedCommitAgainstOldest, "Diff Selected Against Oldest", "Diff selected commit against the oldest one", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
