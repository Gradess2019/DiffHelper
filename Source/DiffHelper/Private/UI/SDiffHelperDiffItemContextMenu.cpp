// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffItemContextMenu.h"

#include "DiffHelperCommands.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffItemContextMenu::BuildMenu(FMenuBuilder& InMenuBuilder)
{
	InMenuBuilder.BeginSection("DiffHelper.DiffPanel.ContextMenu");
	InMenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().OpenLocation);
	InMenuBuilder.AddMenuEntry(FDiffHelperCommands::Get().DiffAgainstTarget);
	InMenuBuilder.EndSection();
}

TSharedPtr<FUICommandList> SDiffHelperDiffItemContextMenu::GetCommandList(const TWeakObjectPtr<UDiffHelperTabController>& InController)
{
	return InController.IsValid() ? InController->GetDiffPanelCommands() : nullptr;
}
