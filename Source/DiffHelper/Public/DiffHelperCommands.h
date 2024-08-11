// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "DiffHelperStyle.h"

class FDiffHelperCommands : public TCommands<FDiffHelperCommands>
{
public:
	FDiffHelperCommands()
		: TCommands<FDiffHelperCommands>(TEXT("DiffHelper"), NSLOCTEXT("Contexts", "DiffHelper", "DiffHelper Plugin"), NAME_None, FDiffHelperStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenDiffWindow;
	TSharedPtr<FUICommandInfo> CreateNewDiff;

	// Diff panel commands
	TSharedPtr<FUICommandInfo> GroupByDirectory;
	TSharedPtr<FUICommandInfo> ExpandAll;
	TSharedPtr<FUICommandInfo> CollapseAll;

	// Commit panel commands
	TSharedPtr<FUICommandInfo> DiffAgainstTarget;
	TSharedPtr<FUICommandInfo> DiffSelectedCommits;
	TSharedPtr<FUICommandInfo> DiffSelectedCommitAgainstNext;
	TSharedPtr<FUICommandInfo> DiffSelectedCommitAgainstPrevious;
	TSharedPtr<FUICommandInfo> DiffSelectedCommitAgainstNewest;
	TSharedPtr<FUICommandInfo> DiffSelectedCommitAgainstOldest;
};
