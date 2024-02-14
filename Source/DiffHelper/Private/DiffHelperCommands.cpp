// Copyright Epic Games, Inc. All Rights Reserved.

#include "DiffHelperCommands.h"

#define LOCTEXT_NAMESPACE "FDiffHelperModule"

void FDiffHelperCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "DiffHelper", "Execute DiffHelper action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
