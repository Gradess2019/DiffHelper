// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperWindow.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"
#include "UI/SDiffHelperBranchPicker.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SDiffHelperWindow::~SDiffHelperWindow()
{
	Controller->Deinit();
	Controller = nullptr;
}

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
				SNew(SBox)
				.WidthOverride(250.f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SNew(SDiffHelperBranchPicker)
						.Controller(Controller)
						.Hint(LOCTEXT("DiffHelperSourceBranchHint", "Select source branch..."))
					]
					+ SVerticalBox::Slot()
					[
						SNew(SDiffHelperBranchPicker)
						.Controller(Controller)
						.Hint(LOCTEXT("DiffHelperTargetBranchHint", "Select target branch..."))
					]
				]
			]
		]);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
