// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperWindow.h"
#include "UI/SDiffHelperPickerPanel.h"
#include "UI/DiffHelperTabController.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabModel.h"
#include "UI/SDiffHelperDiffViewer.h"

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

	// TODO: Test data
	const auto* Model = Controller->GetModel();
	const auto Branches = Model->Branches;

	Controller->SetSourceBranch(Branches[1]);
	Controller->SetTargetBranch(Branches[0]);
	Controller->CollectDiff();
	
	const auto Diff = Model->Diff;

	// SWindow::Construct(
	// 	SWindow::FArguments()
	// 	.Title(LOCTEXT("DiffHelperWindowTitle", "Diff Helper"))
	// 	.ClientSize(FVector2f(800, 600))
	// 	[
	// 		SNew(SDiffHelperPickerPanel)
	// 		.Controller(Controller)
	// 		.OnShowDiff(this, &SDiffHelperWindow::OnShowDiff)
	// 	]);
	SWindow::Construct(
		SWindow::FArguments()
		.Title(LOCTEXT("DiffHelperWindowTitle", "Diff Helper"))
		.ClientSize(FVector2f(800, 600))
		[
			SNew(SDiffHelperDiffViewer)
			.Controller(Controller)
		]);
}

void SDiffHelperWindow::OnShowDiff()
{
	SetContent(SNullWidget::NullWidget);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
