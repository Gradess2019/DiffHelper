// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperBaseCommitContextMenu.h"

#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperBaseCommitContextMenu::Construct(const FArguments& InArgs)
{
	if (!ensure(InArgs._Controller.IsValid())) { return; }

	const auto& CommandList = GetCommandList(InArgs._Controller);
	FMenuBuilder MenuBuilder(true, CommandList);
	MenuBuilder.SetStyle(&FAppStyle::Get(), "PinnedCommandList");

	BuildMenu(MenuBuilder);

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
