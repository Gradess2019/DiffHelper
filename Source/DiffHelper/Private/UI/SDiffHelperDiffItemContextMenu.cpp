// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffItemContextMenu.h"

#include "DiffHelperCommands.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffItemContextMenu::BuildMenu(FMenuBuilder& InMenuBuilder)
{
	InMenuBuilder.BeginSection("DiffHelper.CommitPanel.ContextMenu");
	InMenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffAgainstTarget);
	InMenuBuilder.EndSection();
}
