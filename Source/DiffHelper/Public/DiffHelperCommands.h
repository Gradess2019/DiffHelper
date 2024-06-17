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
};
