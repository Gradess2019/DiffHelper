// Copyright Epic Games, Inc. All Rights Reserved.

#include "DiffHelperCommands.h"

#define LOCTEXT_NAMESPACE "FDiffHelperModule"

void FDiffHelperCommands::RegisterCommands()
{
	UI_COMMAND(OpenDiffWindow, "DiffHelper", "Open diff helper window", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CreateNewDiff, "New...", "Create a new diff", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
