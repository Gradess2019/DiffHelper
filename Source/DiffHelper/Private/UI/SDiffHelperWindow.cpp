// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperWindow.h"
#include "DiffHelperCommands.h"
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

	const auto DiffPicker = CreateDiffPicker();
	SWindow::Construct(
		SWindow::FArguments()
		.Title(LOCTEXT("DiffHelperWindowTitle", "Diff Helper"))
		.ClientSize(FVector2f(800, 600))
		[
			DiffPicker
		]
	);
}

TSharedRef<SWidget> SDiffHelperWindow::CreateStartupView()
{
	const auto DiffPicker = CreateDiffPicker();
	return DiffPicker;
}

TSharedRef<SWidget> SDiffHelperWindow::CreateDiffView()
{
	const auto Menu = CreateMenu();
	const auto DiffViewer = CreateDiffViewer();

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			Menu
		]
		+ SVerticalBox::Slot()
		[
			DiffViewer
		];
}

TSharedRef<SWidget> SDiffHelperWindow::CreateMenu()
{
	TSharedPtr<FUICommandList> CommandList = MakeShareable(new FUICommandList());

	CommandList->MapAction(
		FDiffHelperCommands::Get().CreateNewDiff,
		FExecuteAction::CreateRaw(this, &SDiffHelperWindow::Reset)
	);

	FMenuBarBuilder MenuBarBuilder(CommandList);
	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("MenuLabel", "Diff"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateLambda([](FMenuBuilder& Builder)
		{
			Builder.AddMenuEntry(FDiffHelperCommands::Get().CreateNewDiff);
		})
	);

	TSharedRef<SWidget> MenuBarWidget = MenuBarBuilder.MakeWidget();
	return MenuBarWidget;
}

TSharedRef<SWidget> SDiffHelperWindow::CreateDiffPicker()
{
	return SNew(SDiffHelperPickerPanel)
		.Controller(Controller)
		.OnShowDiff(this, &SDiffHelperWindow::OnShowDiff);
}

TSharedRef<SWidget> SDiffHelperWindow::CreateDiffViewer()
{
	return SNew(SDiffHelperDiffViewer)
		.Controller(Controller);
}

void SDiffHelperWindow::Reset()
{
	const auto StartupView = CreateStartupView();
	SetContent(StartupView);

	SetTitle(LOCTEXT("DiffHelperWindowTitle", "Diff Helper"));
}

void SDiffHelperWindow::OnShowDiff()
{
	const auto DiffView = CreateDiffView();
	SetContent(DiffView);

	const auto NewTitle = Controller->GetModel()->SourceBranch.Name + " -> " + Controller->GetModel()->TargetBranch.Name;
	SetTitle(FText::FromString(NewTitle));
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
