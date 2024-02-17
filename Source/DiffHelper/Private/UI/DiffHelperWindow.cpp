// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperWindow.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"
#include "UI/SDiffHelperBranchPicker.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperWindow::Construct(const FArguments& InArgs)
{
	Controller = NewObject<UDiffHelperTabController>();
	Controller->Init();

	SWindow::Construct(
		SWindow::FArguments()
		.Title(LOCTEXT("DiffHelperWindowTitle", "Diff Helper"))
		.ClientSize(FVector2f(800, 600))
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				[
					SNew(SDiffHelperBranchPicker)
					.Controller(Controller)
				]
				+ SVerticalBox::Slot()
				[
					SNew(SDiffHelperBranchPicker)
					.Controller(Controller)
				]
			]
		]);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
