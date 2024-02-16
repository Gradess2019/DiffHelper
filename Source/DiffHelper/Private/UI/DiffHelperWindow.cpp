// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperWindow.h"

#include "SlateOptMacros.h"
#include "SSearchableComboBox.h"

#include "UI/SDiffHelperBranchPicker.h"

#define LOCTEXT_NAMESPACE "DiffHelper"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperWindow::Construct(const FArguments& InArgs)
{
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
				]
				+ SVerticalBox::Slot()
				[
					SNew(SDiffHelperBranchPicker)
				]
			]
		]);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
