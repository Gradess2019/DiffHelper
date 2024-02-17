// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperWindow.h"
#include "SDiffHelperPickerPanel.h"
#include "SlateOptMacros.h"
#include "UI/DiffHelperTabController.h"

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
			SNew(SDiffHelperPickerPanel)
			.Controller(Controller)
		]);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
